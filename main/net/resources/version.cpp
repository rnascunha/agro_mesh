#include "esp_log.h"
#include "../coap_engine.hpp"

#include "../../version.hpp"

extern const char* RESOURCE_TAG;

static void get_version_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get temp sensor handler");

	/**
	 * Making the payload
	 */
	char buffer[20];
	snprintf(buffer, 20, "%s %s", hw_version, fw_version);
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	/**
	 * Sending response (always call serialize)
	 */
	response
			.code(CoAP::Message::code::content)
			.add_option(content)
			.payload(buffer)
			.serialize();
}

engine::resource_node res_vesion{"version", get_version_handler};
