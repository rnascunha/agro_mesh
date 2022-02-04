#include "esp_log.h"
#include "../coap_engine.hpp"

#include "../../storage.hpp"
#include "../../modules/job/job_define.hpp"

extern const char* RESOURCE_TAG;

extern Agro::Jobs::runner_type runner;

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

	FILE *f = fopen(runner.path(), "rb");
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
	ESP_LOGI(RESOURCE_TAG, "Called put job handler");

	if(!storage_is_mounted())
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("storage not mounted")
			.serialize();
		return;
	}

	if(request.payload_len % Agro::Jobs::job_type::packet_size)
	{
		response
			.code(CoAP::Message::code::bad_request)
			.payload("invalid payload")
			.serialize();
		return;
	}

	FILE *f = fopen(runner.path(), "wb");
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

	runner.clear();

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

	unlink(runner.path());

	response
		.code(CoAP::Message::code::deleted)
		.serialize();
}

engine::resource_node res_job("job",
								get_job_handler,
								nullptr,
								put_job_handler,
								delete_job_handler);
