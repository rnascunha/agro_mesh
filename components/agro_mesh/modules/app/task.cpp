#include "app.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mbedtls/md.h"

#include "../../net/helper.hpp"
#include "../../net/messages/send.hpp"

#include <string.h>

TaskHandle_t app_task_handler = NULL;
extern const char *base_path;

#define TAG		"app"

#define PAYLOAD_SIZE		CONFIG_BUFFER_TRANSACTION_PAYLOAD

static struct {
	app			data;
	char 		file_name[app_max_name_size + 10];		//base bash + '/'
	std::uint8_t hash[32];
	unsigned 	total_size = 0;
	unsigned 	next_block = 0;

	void reset() noexcept
	{
		data.size = 0;
		total_size = 0;
		next_block = 0;
	}
} app_transfer_data;

static void send_app_info(app_status status) noexcept
{
	CoAP::Error ec;
	Agro::send_info(CoAP::Message::type::confirmable, app_error_string(status), ec);
}

static void fail_app_task(app_status status, TaskHandle_t handle = NULL) noexcept
{
	send_app_info(status);
	unlink(app_transfer_data.file_name);
	vTaskDelete(handle);
	app_task_handler = NULL;
}

static bool calculate_sha256hash(const char* file_name, std::uint8_t* result) noexcept
{
	mbedtls_md_context_t ctx;
	mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
	std::uint8_t payload[512];
	std::size_t size;

	mbedtls_md_init(&ctx);
	mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
	mbedtls_md_starts(&ctx);
	FILE* f = fopen(file_name, "rb");
	if(!f)
	{
		ESP_LOGE(TAG, "Calculate HASH file not found");
		return false;
	}
	do{
		size = fread(payload, sizeof(std::uint8_t), 512, f);
		if(!size) break;
		mbedtls_md_update(&ctx, (const unsigned char *) payload, size);
	}while(true);
	fclose(f);
	mbedtls_md_finish(&ctx, result);
	mbedtls_md_free(&ctx);

	return true;
}

static void request_app_cb(void const* trans,
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
//			CoAP::Debug::print_message_string(*response);
			fail_app_task(app_status::server_response, app_task_handler);
			return;
		}

		Option::option opt;
		if(Option::get_option(*response, opt, Option::code::size2))
		{
			unsigned size = Option::parse_unsigned(opt);
			app_transfer_data.total_size = size;
		}

		if(Option::get_option(*response, opt, Option::code::block2))
		{
			unsigned value = Option::parse_unsigned(opt);
			unsigned //block_size = Option::block_size(value),
					block_num = Option::block_number(value);
					//offset = Option::byte_offset(value);

			if(app_transfer_data.next_block != block_num)
			{
				/**
				 * Probabily a retransmited block was sent before receive the ack
				 */
				ESP_LOGE(TAG, "Invalid block number (out of order block %u)", block_num);
				return;
			}

			if(block_num == 0)
			{
				unlink(app_transfer_data.file_name);
			}
			FILE* f = fopen(app_transfer_data.file_name, "a+b");
			if(!f)
			{
				ESP_LOGE(TAG, "[request_app_cb] error opening file = [%s]", app_transfer_data.file_name);
				fail_app_task(app_status::file_list_error, app_task_handler);
				return;
			}
			fwrite(response->payload, 1, response->payload_len, f);
			fclose(f);
			app_transfer_data.data.size += response->payload_len;

			ESP_LOGD(TAG, "Written image length %u", app_transfer_data.data.size);

			if(!Option::more(value))
			{
				vTaskResume(app_task_handler);
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
			app_transfer_data.next_block = block_num + 1;

			/**
			 * Making a request to get the next block.
			 */
			engine::transaction_t const* transac = static_cast<engine::transaction_t const*>(trans);
			Option::node block2_op{Option::code::block2, n_value};
			CoAP::Message::Option::node path_op{CoAP::Message::Option::code::uri_path, "app"};

			/**
			 * Making request to ask for next block
			 */
			engine::request req{transac->endpoint()};
			req
				.header(CoAP::Message::type::confirmable, code::get)
				.add_option(block2_op)
				.add_option(path_op)
				.payload(app_transfer_data.data.name)
				.callback(request_app_cb);
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
		if(app_task_handler)
		{
			fail_app_task(app_status::receive_timeout, app_task_handler);
		}
		else
		{
			send_app_info(app_status::receive_timeout);
		}
	}
}

static void app_task(void*)
{
	app_transfer_data.reset();
	app mapp;
	app_status status = get_app(mapp, app_transfer_data.data.name);
	if(status == app_status::success)
	{
		CoAP::Error ec;
		Agro::send_report(CoAP::Message::type::confirmable,
				report_type::warning,
				"App already installed",
				ec);
		app_task_handler = NULL;
		vTaskDelete(NULL);
		return;
	}

	//Making dummy endpoint
	mesh_addr_t addr{};
	addr.mip.port = htons(5683);
	engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};
	//Making request
	unsigned block2;
	CoAP::Message::Option::make_block(block2, 0, 0, PAYLOAD_SIZE);
	CoAP::Message::Option::node block2_op{CoAP::Message::Option::code::block2, block2};
	CoAP::Message::Option::node uri_path{CoAP::Message::Option::code::uri_path, "app"};

	engine::request req{ep};
	req.header(CoAP::Message::type::confirmable, CoAP::Message::code::get)
		.add_option(uri_path)
		.add_option(block2_op)
		.payload(app_transfer_data.data.name)
		.callback(request_app_cb);

	CoAP::Error ec;
	Agro::coap_send(req, ec);
	if(ec)
	{
		ESP_LOGE(TAG, "Error app send request [%d/%s]", ec.value(), ec.message());
		app_task_handler = NULL;
		vTaskDelete(NULL);
		return;
	}

	vTaskSuspend(NULL);

	ESP_LOGI(TAG, "Finished transfer: total=%u/tranfered=%u",
			app_transfer_data.total_size, app_transfer_data.data.size);

	/**
	 * Checking app hash
	 */
	std::uint8_t hash[32];
	calculate_sha256hash(app_transfer_data.file_name, hash);
	if(std::memcmp(app_transfer_data.hash, hash, 32) != 0)
	{
		/**
		 * Hash check failed
		 */
		Agro::send_report(CoAP::Message::type::confirmable,
				report_type::error,
				"App hash not met", ec);
		unlink(app_transfer_data.file_name);
	}
	else
	{
		/**
		 * Adding app to list
		 */
		add_app(app_transfer_data.data.name, app_transfer_data.data.size, hash);
		Agro::send_report(CoAP::Message::type::confirmable,
				report_type::success,
				"App installed", ec);
		send_app_list(coap_engine, CoAP::Message::type::confirmable, ec);
	}

	app_task_handler = NULL;
	vTaskDelete(NULL);
}

bool init_app_task(const char* name, const std::uint8_t* hash) noexcept
{
	/**
	 * Check if task is already running
	 */
	if(app_task_handler)
	{
		return false;
	}

	std::strcpy(app_transfer_data.data.name, name);

	//Making full file path
	std::strcpy(app_transfer_data.file_name, base_path);
	std::strcat(app_transfer_data.file_name, "/");
	std::strcat(app_transfer_data.file_name, name);

	//Copying hash
	std::memcpy(app_transfer_data.hash, hash, 32);

	xTaskCreate(app_task, "app", 3072, NULL, 5, &app_task_handler);

	return true;
}

/**
 * Old debug functions
 */
//static void print_hash(const std::uint8_t* hash) noexcept
//{
//	for(int i = 0; i < 32; i++)
//		printf("%02X ", hash[i]);
//}

//static void print_app_struct()
//{
//	printf("App struct:\n\tname[%s]\n\tsize[%u]\n\tfile_name[%s]\n\thash[",
//			app_transfer_data.data.name,
//			app_transfer_data.data.size,
//			app_transfer_data.file_name);
//	print_hash(app_transfer_data.hash);
//	printf("]\n\ttotal_size[%u]\n\tnext_block[%u]\n",
//			app_transfer_data.total_size,
//			app_transfer_data.next_block);
//
//}
