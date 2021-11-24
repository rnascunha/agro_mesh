#include "esp_log.h"
#include "../coap_engine.hpp"
#include "esp_timer.h"
#include "esp_system.h"

#if CONFIG_COAP_RESOURCE_UPTIME || CONFIG_COAP_RESOURCE_RESET_REASON || CONFIG_COAP_RESOURCE_REBOOT
extern const char* RESOURCE_TAG;
#endif

#if CONFIG_COAP_RESOURCE_UPTIME
/**
 * \/time [GET]
 *
 * Retrieve uptime of device in microseconds (us)
 */
static void get_uptime_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get uptime handler");

	/**
	 * creating option content_format
	 */
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	char time[20];
	std::snprintf(time, 20, "%lld", esp_timer_get_time());

	/**
	 * Making response (always call serialize)
	 */
	response
			.code(CoAP::Message::code::content)
			.add_option(content)
			.payload(time)
			.serialize();
}

engine::resource_node	res_uptime("uptime",
							get_uptime_handler);

#endif /* CONFIG_COAP_RESOURCE_UPTIME */

#if CONFIG_COAP_RESOURCE_RESET_REASON

static void get_reset_reason_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get time handler");

	/**
	 * creating option content_format
	 */
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	char reason[10];
	std::snprintf(reason, 10, "%d", static_cast<int>(esp_reset_reason()));

	response
			.code(CoAP::Message::code::content)
			.add_option(content)
			.payload(reason)
			.serialize();
}

engine::resource_node	res_reset_reason("reset_reason",
							get_reset_reason_handler);

#endif /* CONFIG_COAP_RESOURCE_RESET_REASON */

#if CONFIG_COAP_RESOURCE_REBOOT

static void post_reboot_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called put time handler");

	esp_restart();
	response
		.code(CoAP::Message::code::success)
		.serialize();
}

engine::resource_node	res_reboot("reboot",
							static_cast<engine::resource::callback_t*>(nullptr),
							post_reboot_handler);

#endif /* CONFIG_COAP_RESOURCE_REBOOT */
