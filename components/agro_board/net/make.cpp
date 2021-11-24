#include "make.hpp"
#include "../sensor/read_sensor.hpp"
#include "agro/helper.hpp"
#include "esp_ota_ops.h"
#include "types.hpp"

#include "agro/version.hpp"

extern std::uint8_t temp_sensor_count;

std::size_t make_board_config(void* buffer, std::size_t buffer_len) noexcept
{
	const esp_app_desc_t * desc = esp_ota_get_app_description();

	std::size_t fw_size = std::strlen(desc->version),
				hw_size = std::strlen(Agro::hw_version);
	if(buffer_len < (sizeof(board_config) + fw_size + 1 + hw_size))
	{
		return 0;
	}
	board_config bc;

	bc.has_rtc = 0;//device_clock.has_rtc() ? 1 : 0;
	bc.has_temp_sensor = temp_sensor_count > 0 ? 1 : 0;

	std::uint8_t* bu8 = static_cast<std::uint8_t*>(buffer);
	std::memcpy(bu8, &bc, sizeof(bc));
	std::memcpy(bu8 + sizeof(bc), desc->version, fw_size);
	bu8[sizeof(bc) + fw_size] = '|';
	std::memcpy(bu8 + sizeof(bc) + 1 + fw_size, Agro::hw_version, hw_size);

	return sizeof(bc) + fw_size + 1 + hw_size;
}

std::size_t make_sensors_data(std::uint8_t* data, std::size_t size) noexcept
{
	Sensor_Builder builder(data, size);
	if(temp_sensor_count > 0)
	{
		float temp = read_temperature_retry(2);
		if(temp != INVALID_TEMPERATURE)
		{
			builder.add(SENSOR_TYPE_TEMP, temp);
		}
	}

	wifi_ap_record_t ap;
	esp_wifi_sta_get_ap_info(&ap);
	builder.add(SENSOR_TYPE_RSSI, ap.rssi);

	unsigned bit_array = read_gpios();

	builder.add(SENSOR_TYPE_GPIOS, bit_array);

	return builder.size();
}
