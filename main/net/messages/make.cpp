#include "make.hpp"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_ota_ops.h"

#include <cstring>
#include "../../version.hpp"

#include "dallas_temperature.h"
#include "gpio.h"
#include "../../types/rtc_time.hpp"
#include "../../types/sensor_type.hpp"
#include "../../types/sensor_type_list.hpp"

//extern bool rtc_present;
extern std::uint8_t temp_sensor_count;

//extern DS3231 rtc;
extern RTC_Time device_clock;
extern Dallas_Temperature temp_sensor;
extern GPIO_Basic water_level[];
extern GPIO_Basic gpio_generic[];
extern GPIO_Basic ac_load[];

#define INVALID_TEMPERATURE		(-127.0)

static float read_temperature_retry(unsigned retry_times) noexcept
{
	do{
		temp_sensor.requestTemperatures();
		float temp = temp_sensor.getTempCByIndex(0);
		if(temp != INVALID_TEMPERATURE)
			return temp;
	}while(retry_times--);

	return INVALID_TEMPERATURE;
}


config* make_config(config& cfg) noexcept
{
	mesh_cfg_t cfg_;
	wifi_ap_record_t ap;

	esp_mesh_get_config(&cfg_);
	esp_wifi_sta_get_ap_info(&ap);

	esp_read_mac(cfg.mac_ap.addr, ESP_MAC_WIFI_SOFTAP);
	std::memcpy(&cfg.net_id.addr, &cfg_.mesh_id.addr, 6);
	cfg.channel_config = cfg_.channel;
	cfg.channel_conn = ap.primary;

	return &cfg;
}

std::size_t make_board_config(void* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept
{
	const esp_app_desc_t * desc = esp_ota_get_app_description();

	std::size_t fw_size = std::strlen(desc->version),
				hw_size = std::strlen(hw_version);
	if(buffer_len < (sizeof(board_config) + fw_size + 1 + hw_size))
	{
		ec = CoAP::errc::insufficient_buffer;
		return 0;
	}
	board_config bc;

	bc.has_rtc = device_clock.has_rtc() ? 1 : 0;
	bc.has_temp_sensor = temp_sensor_count > 0 ? 1 : 0;

	std::uint8_t* bu8 = static_cast<std::uint8_t*>(buffer);
	std::memcpy(bu8, &bc, sizeof(bc));
	std::memcpy(bu8 + sizeof(bc), desc->version, fw_size);
	bu8[sizeof(bc) + fw_size] = '|';
	std::memcpy(bu8 + sizeof(bc) + 1 + fw_size, hw_version, hw_size);

	return sizeof(bc) + fw_size + 1 + hw_size;
}

size_t make_sensors_data(uint8_t* data, size_t size) noexcept
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

	unsigned bit_array = 0;

	bit_array |= (water_level[0].read() << 0);
	bit_array |= (water_level[1].read() << 1);
	bit_array |= (water_level[2].read() << 2);
	bit_array |= (water_level[3].read() << 3);

	bit_array |= (gpio_generic[0].read() << 4);
	bit_array |= (gpio_generic[1].read() << 5);
	bit_array |= (gpio_generic[2].read() << 6);
	bit_array |= (gpio_generic[3].read() << 7);

	bit_array |= (ac_load[0].read() << 8);
	bit_array |= (ac_load[1].read() << 9);
	bit_array |= (ac_load[2].read() << 10);

	builder.add(SENSOR_TYPE_GPIOS, bit_array);

	return builder.size();
}
