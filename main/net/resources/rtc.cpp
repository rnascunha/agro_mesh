#include "esp_log.h"
#include "../coap_engine.hpp"
#include "../../helper/convert.hpp"

#include "datetime.h"
#include "ds3231.hpp"

extern DS3231 rtc;
extern bool rtc_present;

extern const char* RESOURCE_TAG;

void get_rtc_time_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get rtc time handler");

	if(rtc_present == 0)
	{
		response
			.code(CoAP::Message::code::not_implemented)
			.payload("rtc not detected")
			.serialize();
		return;
	}

	DateTime dt;
	rtc.getDateTime(&dt);

	/**
	 * Making the payload
	 */
	char buffer[20];
	snprintf(buffer, 20, "%u", dt.getUnixTime());
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

void put_rtc_time_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called put rtc time handler");

	if(rtc_present == 0)
	{
		response
			.code(CoAP::Message::code::not_implemented)
			.payload("rtc not detected")
			.serialize();
		return;
	}

	ESP_LOGI(RESOURCE_TAG, "Payload[%zu]: %.*s",
			request.payload_len,
			request.payload_len, static_cast<const char*>(request.payload));
	if(!request.payload || request.payload_len == 0)
	{
		response
			.code(CoAP::Message::code::bad_request)
			.payload("data not set")
			.serialize();
		return;
	}

	DateTime dt;
	std::uint32_t unix_time = strntoll(static_cast<const char*>(request.payload), request.payload_len);
	dt.setUnixTime(unix_time);
	rtc.setDateTime(&dt);

	response
			.code(CoAP::Message::code::changed)
			.serialize();
}

engine::resource_node res_rtc_time("rtc",
									get_rtc_time_handler,
									nullptr,
									put_rtc_time_handler);
