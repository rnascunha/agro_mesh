/**
 * OTA update task
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "driver/gpio.h"
#include "errno.h"

#include "../../net/helper.hpp"

#define BUFFSIZE 1024
#define HASH_LEN 32 /* SHA-256 digest length */

static const char *TAG = "OTA";
extern const std::size_t image_name_max_size = 31;

#define PAYLOAD_SIZE		CONFIG_BUFFER_TRANSACTION_PAYLOAD

TaskHandle_t ota_task_handler = NULL;

static int64_t init_time = 0;
static int64_t end_time = 0;

struct OTA_update{
	bool checked = false;
	const esp_partition_t *update_partition = NULL;
	esp_ota_handle_t handler = 0;
	std::size_t total_bytes_transfer = 0;
	unsigned next_block = 0;
	char image_name[image_name_max_size];

	void reset() noexcept
	{
		checked = false;
		update_partition = NULL;
		handler = 0;
		total_bytes_transfer = 0;
		next_block = 0;
	}
};

static OTA_update ota_data;

enum class ota_status{
	success = 0,
	image_already_rollback,
	same_version,
	update_begin,
	update_end,
	image_too_big,
	ota_write,
	update_partition_not_found,
	image_corrupted,
	set_boot,
	reveice_timeout,
	server_response_error
};

static const char* ota_error(ota_status status) noexcept
{
	switch(status)
	{
		case ota_status::success: return "success";
		case ota_status::image_already_rollback: return "image already rolledback";
		case ota_status::same_version: return "update same version";
		case ota_status::update_begin: return "update begin error";
		case ota_status::update_end: return "update end error";
		case ota_status::image_too_big: return "image too big";
		case ota_status::ota_write: return "image write";
		case ota_status::update_partition_not_found: return "partition not found";
		case ota_status::image_corrupted: return "image validation fail";
		case ota_status::set_boot: return "set boot error";
		case ota_status::reveice_timeout: return "receive packet timeout";
		case ota_status::server_response_error: return "server response error";
		default:
			break;
	}
	return "undefined";
}

static void send_ota_info(ota_status status) noexcept
{
	CoAP::Error ec;
	Agro::send_info(CoAP::Message::type::confirmable, ota_error(status), ec);
}

static void fail_ota_task(ota_status status, TaskHandle_t handle = NULL) noexcept
{
	send_ota_info(status);
	vTaskDelete(handle);
	ota_task_handler = NULL;
}

template<bool CheckRunningVersion = true>
static ota_status fisrt_block_check(CoAP::Message::message const& response) noexcept
{
	if (ota_data.checked == false)
	{
		esp_app_desc_t new_app_info;
		if (response.payload_len > sizeof(esp_image_header_t) +
									sizeof(esp_image_segment_header_t) +
									sizeof(esp_app_desc_t))
		{
			// check current version with downloading
			memcpy(&new_app_info,
					&static_cast<const uint8_t*>(response.payload)[sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t)],
					sizeof(esp_app_desc_t));
			ESP_LOGI(TAG, "New firmware version: %s", new_app_info.version);

			const esp_partition_t *running = esp_ota_get_running_partition();
			esp_app_desc_t running_app_info;
			if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK)
			{
				ESP_LOGI(TAG, "Running firmware version: %s", running_app_info.version);
			}

			const esp_partition_t* last_invalid_app = esp_ota_get_last_invalid_partition();
			esp_app_desc_t invalid_app_info;
			if (esp_ota_get_partition_description(last_invalid_app, &invalid_app_info) == ESP_OK)
			{
				ESP_LOGI(TAG, "Last invalid firmware version: %s", invalid_app_info.version);
			}

			// check current version with last invalid partition
			if (last_invalid_app != NULL)
			{
				if (memcmp(invalid_app_info.version, new_app_info.version, sizeof(new_app_info.version)) == 0) {
					ESP_LOGW(TAG, "New version is the same as invalid version.");
					ESP_LOGW(TAG, "Previously, there was an attempt to launch the firmware with %s version, but it failed.", invalid_app_info.version);
					ESP_LOGW(TAG, "The firmware has been rolled back to the previous version.");
					return ota_status::image_already_rollback;
				}
			}

			if constexpr(CheckRunningVersion)
			{
				if (memcmp(new_app_info.version, running_app_info.version, sizeof(new_app_info.version)) == 0)
				{
					ESP_LOGW(TAG, "Current running version is the same as a new. We will not continue the update.");
					return ota_status::same_version;
				}
			}
			ota_data.checked = true;

			esp_err_t err;
			err = esp_ota_begin(ota_data.update_partition, OTA_WITH_SEQUENTIAL_WRITES, &ota_data.handler);
			if (err != ESP_OK)
			{
				ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
				esp_ota_abort(ota_data.handler);
				return ota_status::update_begin;
			}
			ESP_LOGI(TAG, "esp_ota_begin succeeded");
		}
	}
	return ota_status::success;
}

static void request_ota_cb(void const* trans,
		CoAP::Message::message const* response,
		void*) noexcept
{
	ESP_LOGD(TAG, "Request callback called...");

	/**
	 * Checking response
	 */
	if(response)
	{
		ESP_LOGD(TAG, "Response received! [%zu]", response->payload_len);

		using namespace CoAP::Message;
		if(CoAP::Message::is_error(response->mcode))
		{
			fail_ota_task(ota_status::server_response_error, ota_task_handler);
			return;
		}

		/**
		 * The size2 option, if present, informs the total size of the
		 * data that will be transfered
		 */
		Option::option opt;
		if(Option::get_option(*response, opt, Option::code::size2))
		{
			/**
			 * Checking if the size fits our buffer
			 */
			unsigned size = Option::parse_unsigned(opt);
			if(size > ota_data.update_partition->size)
			{
				ESP_LOGE(TAG, "Data size to bigger than OTA partition! Interrupting transfer\n");
				fail_ota_task(ota_status::image_too_big, ota_task_handler);
				return;
			}
		}

		if(Option::get_option(*response, opt, Option::code::block2))
		{
			unsigned value = Option::parse_unsigned(opt);
			unsigned //block_size = Option::block_size(value),
					block_num = Option::block_number(value);
					//offset = Option::byte_offset(value);

			if(ota_data.next_block != block_num)
			{
				/**
				 * Probabily a retransmited block was sent before receive the ack
				 */
				ESP_LOGE(TAG, "Invalid block number (out of order block %u)", block_num);
				return;
			}

			if(block_num == 0)
			{
				ota_status status = fisrt_block_check(*response);
				if(status != ota_status::success)
				{
					ESP_LOGE(TAG, "Error processing first block");
					fail_ota_task(status, ota_task_handler);
					return;
				}
			}
			esp_err_t err;
			err = esp_ota_write(ota_data.handler, response->payload, response->payload_len);
			if (err != ESP_OK)
			{
				esp_ota_abort(ota_data.handler);
				fail_ota_task(ota_status::ota_write, ota_task_handler);
				return;
			}
			ota_data.total_bytes_transfer += response->payload_len;
			ESP_LOGD(TAG, "Written image length %d", ota_data.total_bytes_transfer);

			if(!Option::more(value))
			{
				vTaskResume(ota_task_handler);
				return;
			}

			/**
			 * Making next request block option
			 *
			 * The last parameter must be a value from 16 to 1024, power of 2,
			 * i.e: 16, 32, 64, 128, 256, 512 or 1024.
			 *
			 * Any other value will return false
			 */
			unsigned n_value;
			Option::make_block(n_value, block_num + 1/*Option::block_number(value) + 1*/, 0, PAYLOAD_SIZE);
			ota_data.next_block = block_num + 1;

			/**
			 * Making a request to get the next block.
			 */
			engine::transaction_t const* transac = static_cast<engine::transaction_t const*>(trans);
			Option::node block2_op{Option::code::block2, n_value};
			CoAP::Message::Option::node path_op{CoAP::Message::Option::code::uri_path, "ota"};

			/**
			 * Making request to ask for next block
			 */
			engine::request req{transac->endpoint()};
			req
				.header(CoAP::Message::type::confirmable, code::get)
				.add_option(block2_op)
				.add_option(path_op)
				.payload(ota_data.image_name)
				.callback(request_ota_cb);
			CoAP::Error ec;

			/**
			 * Sending
			 */
			coap_engine.send(req, ec);
		}
		else
		{
			/**
			 * If not block wise transfer, just print the data
			 */
			ESP_LOGE(TAG, "Response doesn't have op Block2\n");
		}
	}
	else
	{
		/**
		 * Function timeout or transaction was cancelled
		 */
		ESP_LOGI(TAG, "Response NOT received");
		if(ota_task_handler)
		{
			fail_ota_task(ota_status::reveice_timeout, ota_task_handler);
		}
		else
		{
			send_ota_info(ota_status::reveice_timeout);
		}
	}
}

void ota_task(void*)
{
	init_time = esp_timer_get_time();

	ota_data.reset();

	ESP_LOGI(TAG, "Starting OTA example");

	const esp_partition_t *configured = esp_ota_get_boot_partition();
	const esp_partition_t *running = esp_ota_get_running_partition();

	if (configured != running)
	{
		ESP_LOGW(TAG, "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x",
				 configured->address, running->address);
		ESP_LOGW(TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
	}
	ESP_LOGI(TAG, "Running partition type %d subtype %d (offset 0x%08x)",
			 running->type, running->subtype, running->address);

	ota_data.update_partition = esp_ota_get_next_update_partition(NULL);
	if(!ota_data.update_partition)
	{
		ESP_LOGE(TAG, "Invalid elegiable update OTA partition");
		fail_ota_task(ota_status::update_partition_not_found);
		return;
	}
	ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x",
			 ota_data.update_partition->subtype, ota_data.update_partition->address);

	//Making dummy endpoint
	mesh_addr_t addr{};
	addr.mip.port = htons(5683);
	engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};
	//Making request
	unsigned block2;
	CoAP::Message::Option::make_block(block2, 0, 0, PAYLOAD_SIZE);
	CoAP::Message::Option::node block2_op{CoAP::Message::Option::code::block2, block2};
	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, "ota"};

	engine::request req{ep};
	req.header(CoAP::Message::type::confirmable, CoAP::Message::code::get)
		.add_option(uri_path)
		.add_option(block2_op)
		.payload(ota_data.image_name)
		.callback(request_ota_cb);

	CoAP::Error ec;
	Agro::coap_send(req, ec);
	if(ec)
	{
		ESP_LOGE(TAG, "Error ota send request [%d/%s]", ec.value(), ec.message());
		ota_task_handler = NULL;
		vTaskDelete(NULL);
		return;
	}

	vTaskSuspend(NULL);

	esp_err_t err;
	err = esp_ota_end(ota_data.handler);
	if (err != ESP_OK)
	{
		if (err == ESP_ERR_OTA_VALIDATE_FAILED)
		{
			ESP_LOGE(TAG, "Image validation failed, image is corrupted");
			fail_ota_task(ota_status::image_corrupted);
		}
		else
		{
			ESP_LOGE(TAG, "esp_ota_end failed (%s)!", esp_err_to_name(err));
			fail_ota_task(ota_status::update_end);
		}
		return;
	}
	err = esp_ota_set_boot_partition(ota_data.update_partition);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)!", esp_err_to_name(err));
		fail_ota_task(ota_status::set_boot);
		return;
	}
	end_time = esp_timer_get_time();
	ESP_LOGI(TAG, "Update time [%lld->%lld]: %lld", init_time, end_time, end_time - init_time);

	send_info(coap_engine, CoAP::Message::type::confirmable, "Update success", ec);

	ota_task_handler = NULL;
	vTaskDelete(NULL);
}

void init_ota_task(const char* name, std::size_t name_size) noexcept
{
	std::memcpy(ota_data.image_name, name, name_size);
	ota_data.image_name[name_size] = '\0';

    xTaskCreate(ota_task, "ota", 8192, NULL, 5, &ota_task_handler);
}
