#include "esp_log.h"
#include "../coap_engine.hpp"
#include "../../types/app/app.hpp"
#include <stdio.h>
#include <string.h>

extern const char* RESOURCE_TAG;

static char* make_name(char* name, const void* data, unsigned name_size) noexcept
{
	unsigned size = name_size > app_max_name_size - 1 ? app_max_name_size - 1 : name_size;
	std::memcpy(name, data, size);
	name[size] = '\0';

	return name;
}

static void get_app_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get app handler");

	std::uint8_t buffer[engine::packet_size];
	char name[app_max_name_size];
	const void* value = nullptr;
	unsigned length;
	if(CoAP::Message::query_by_key(request, "name", &value, length) || !length)
	{
		app mapp;
		app_status status = get_app(mapp, make_name(name, value, length));
		if(status != app_status::success)
		{
			response
				.code(CoAP::Message::code::not_found)
				.payload(app_error_string(status))
				.serialize();
			return;
		}
		response
			.code(CoAP::Message::code::content)
			.payload(static_cast<const void*>(&mapp), sizeof(app));
		return;
	}

	unsigned size;
	app_status status = get_app_list(buffer, engine::packet_size, size);
	if(status != app_status::success)
	{
		response
			.code(CoAP::Message::code::not_found)
			.payload(app_error_string(status))
			.serialize();
		return;
	}

	response
			.code(CoAP::Message::code::content)
			.payload(buffer, size)
			.serialize();
}

static void post_app_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called post app handler");

	char name[app_max_name_size];
	if(!request.payload || !request.payload_len)
	{
		response
			.code(CoAP::Message::code::precondition_failed)
			.payload("app not defiend")
			.serialize();
		return;
	}

	if(!init_app_task(make_name(name, request.payload, request.payload_len)))
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("app other loading")
			.serialize();
		return;
	}

	response
			.code(CoAP::Message::code::changed)
			.serialize();
}

static void put_app_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called put app handler");

	if(!request.payload || !request.payload_len)
	{
		response
			.code(CoAP::Message::code::precondition_failed)
			.payload("app not defined")
			.serialize();
		return;
	}

	int arg = 0;
	const void* value = nullptr;
	unsigned length;
	if(CoAP::Message::query_by_key(request, "arg", &value, length) && length)
	{
		arg = std::strtoll(static_cast<const char*>(value), nullptr, 10);
	}

	int ret;
	char name[app_max_name_size];
	app_status status = execute_app(make_name(name, request.payload, request.payload_len), arg, ret);
	if(status != app_status::success)
	{
		response
			.code(CoAP::Message::code::not_found)
			.payload(app_error_string(status))
			.serialize();
		return;
	}

	ESP_LOGI(RESOURCE_TAG, "Returned %d", ret);
	response
		.code(CoAP::Message::code::success)
		.payload(&ret, sizeof(ret))
		.serialize();
}

static void delete_app_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called delete app handler");

	if(!request.payload || !request.payload_len)
	{
		response
			.code(CoAP::Message::code::precondition_failed)
			.payload("app not defined")
			.serialize();
		return;
	}

	char name[app_max_name_size];
	app_status status = delete_app(make_name(name, request.payload, request.payload_len));
	if(status != app_status::success)
	{
		response
			.code(CoAP::Message::code::not_found)
			.payload(app_error_string(status))
			.serialize();
		return;
	}

	response
		.code(CoAP::Message::code::success)
		.serialize();
}

engine::resource_node res_app("app",
								get_app_handler,
								post_app_handler,
								put_app_handler,
								delete_app_handler);