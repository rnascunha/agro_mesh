#include "esp_log.h"
#include "../coap_engine.hpp"

#include "dallas_temperature.h"

extern Dallas_Temperature temp_sensor;
extern std::uint8_t temp_sensor_count;

extern const char* RESOURCE_TAG;

/**
 * \/sensors/[temp/light/humidity] [GET]
 *
 * Retrieve sensor value read (a random number here).
 *
 * Same callback to all the sensors
 */
static void get_temp_sensor_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get temp sensor handler");

	if(temp_sensor_count == 0)
	{
		response
			.code(CoAP::Message::code::not_implemented)
			.payload("temp sensor not detected")
			.serialize();
		return;
	}

	temp_sensor.requestTemperatures();
	float temp = temp_sensor.getTempCByIndex(0);

	/**
	 * Making the payload
	 */
	char buffer[20];
	snprintf(buffer, 20, "%f", temp);
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

engine::resource_node res_sensor_temp{"temp", get_temp_sensor_handler};
