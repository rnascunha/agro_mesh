#include "esp_log.h"
#include "../coap_engine.hpp"
#include "esp_ota_ops.h"

#include "../../include/agro/version.hpp"

extern const char* RESOURCE_TAG;

static void get_version_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get version sensor handler");

	/**
	 * Making the payload
	 */
	const esp_app_desc_t * desc = esp_ota_get_app_description();

	char buffer[50];
	snprintf(buffer, 50, "%s|%s", desc->version, Agro::hw_version);
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
