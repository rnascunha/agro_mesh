#include "esp_log.h"
#include "../coap_engine.hpp"

#include "datetime.h"
#include "../../types/rtc_time.hpp"
#include "../../types/job.hpp"
#include "../../storage.hpp"

extern const char* RESOURCE_TAG;

extern const char* job_path;
extern volatile bool job_force_check;

static void get_job_handler(engine::message const& request,
								engine::response& response,
								void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get job handler");

	if(!storage_is_mounted())
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("storage not mounted")
			.serialize();
		return;
	}

	FILE *f = fopen(job_path, "rb");
	if (f == NULL)
	{
		/**
		 * File doesnt exits... no jobs
		 */
		response
			.code(CoAP::Message::code::content)
			.serialize();
		return;
	}

	std::uint8_t data[engine::packet_size];
	std::size_t readed = fread(data, 1, engine::packet_size, f);
	fclose(f);

	response
		.code(CoAP::Message::code::content)
		.payload(data, readed)
		.serialize();
}

static void put_job_handler(engine::message const& request,
								engine::response& response,
								void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called put job handler");

	if(!storage_is_mounted())
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("storage not mounted")
			.serialize();
		return;
	}

	if(request.payload_len % Jobs::job::packet_size)
	{
		response
			.code(CoAP::Message::code::bad_request)
			.payload("invalid payload")
			.serialize();
		return;
	}

	FILE *f = fopen(job_path, "wb");
	if (f == NULL)
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("write error")
			.serialize();
		return;
	}
	fwrite(request.payload, 1, request.payload_len, f);
	fclose(f);

	job_force_check = true;

	response
		.code(CoAP::Message::code::changed)
		.serialize();
}

static void delete_job_handler(engine::message const& request,
								engine::response& response,
								void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called delete job handler");

	if(!storage_is_mounted())
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("storage not mounted")
			.serialize();
		return;
	}

	unlink(job_path);

	response
		.code(CoAP::Message::code::deleted)
		.serialize();
}

engine::resource_node res_job("job",
								get_job_handler,
								nullptr,
								put_job_handler,
								delete_job_handler);
