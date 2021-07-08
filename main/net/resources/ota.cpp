#include "esp_log.h"
#include "../coap_engine.hpp"
#include "esp_ota_ops.h"
#include "../../ota/task.hpp"

#include <cstring>

#include "coap-te-debug.hpp"

extern const char* RESOURCE_TAG;
extern TaskHandle_t ota_task_handler;

static void get_ota_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get ota handler");

	const esp_app_desc_t * desc = esp_ota_get_app_description();

	char buffer[50];
	snprintf(buffer, 50, "%s", desc->version);

	/**
	 * Option
	 */
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	response
			.code(CoAP::Message::code::content)
			.add_option(content)
			.payload(buffer)
			.serialize();
}

static void post_ota_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called post ota handler");
	CoAP::Debug::print_message_string(request);

	if(ota_task_handler)
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("ota task running")
			.serialize();
		return;
	}

	init_ota_task(static_cast<const char*>(request.payload), request.payload_len);

	response
		.code(CoAP::Message::code::created)
		.serialize();
}

engine::resource_node res_ota("ota",
								get_ota_handler,
								post_ota_handler);
