#include "esp_log.h"
#include "../coap_engine.hpp"

#include "gpio.h"

extern const char* RESOURCE_TAG;

extern GPIO_Basic led;

static void get_led_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get led handler");

	/**
	 * Option
	 */
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	/**
	 * Payload
	 */
	char buffer[2];
	snprintf(buffer, 2, "%u", led.read() ? 0 : 1);
	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.payload(buffer)
		.serialize();
}

static void put_led_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called put led handler");

	/**
	 * Querying value (?value=<0|1>)
	 */
	const void* value = nullptr;
	unsigned length;
	if(!CoAP::Message::query_by_key(request, "value", &value, length) || !length)
	{
		/**
		 * query key 'value' not found.
		 */
		response
			.code(CoAP::Message::code::bad_request)
			.payload("value not set")
			.serialize();
		return;
	}

	/**
	 * Setting value
	 */
	char v = *static_cast<char const*>(value);
	led.write(v != '0' ? 0 : 1);

	/**
	 * Payload
	 */
	char buf[2];
	snprintf(buf, 2, "%c", (led.read() ? 0 : 1)  + '0');

	response
		.code(CoAP::Message::code::changed)
		.payload(buf)
		.serialize();
}

engine::resource_node res_led("led",
								get_led_handler,
								nullptr,
								put_led_handler);
