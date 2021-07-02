#include "make.hpp"
#include "esp_system.h"
#include "esp_timer.h"
#include <cstring>
#include "../../version.hpp"

#include "dallas_temperature.h"
#include "datetime.h"
#include "ds3231.hpp"
#include "gpio.h"

extern bool rtc_present;
extern std::uint8_t temp_sensor_count;

extern DS3231 rtc;
extern Dallas_Temperature temp_sensor;
extern GPIO_Basic water_level[];
extern GPIO_Basic ac_load[];


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

status* make_status(status& sts) noexcept
{
	wifi_ap_record_t ap;
	esp_wifi_sta_get_ap_info(&ap);

	sts.rssi = ap.rssi;

	return &sts;
}

std::size_t make_board_config(void* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept
{
	std::size_t fw_size = std::strlen(fw_version),
				hw_size = std::strlen(hw_version);
	if(buffer_len < (sizeof(board_config) + fw_size + 1 + hw_size))
	{
		ec = CoAP::errc::insufficient_buffer;
		return 0;
	}
	board_config bc;

	bc.has_rtc = rtc_present ? 1 : 0;
	bc.has_temp_sensor = temp_sensor_count > 0 ? 1 : 0;

	std::uint8_t* bu8 = static_cast<std::uint8_t*>(buffer);
	std::memcpy(bu8, &bc, sizeof(bc));
	std::memcpy(bu8 + sizeof(bc), fw_version, fw_size);
	bu8[sizeof(bc) + fw_size] = '|';
	std::memcpy(bu8 + sizeof(bc) + 1 + fw_size, hw_version, hw_size);

	return sizeof(bc) + fw_size + 1 + hw_size;
}

sensor_data* make_sensor_data(sensor_data& sen) noexcept
{
	if(temp_sensor_count > 0)
		temp_sensor.requestTemperatures();
	else sen.temp = 0;

	if(rtc_present)
	{
		DateTime dt;
		rtc.getDateTime(&dt);
		sen.time = dt.getUnixTime();
	}
	else
	{
		sen.time = static_cast<std::uint32_t>(esp_timer_get_time() / 1000000);
	}

	if(temp_sensor_count > 0)
	{
		sen.temp = temp_sensor.getTempCByIndex(0);
	}

	sen.wl1 = water_level[0].read();
	sen.wl2 = water_level[1].read();
	sen.wl3 = water_level[2].read();
	sen.wl4 = water_level[3].read();

	sen.ac1 = ac_load[0].read();
	sen.ac2 = ac_load[1].read();
	sen.ac3 = ac_load[2].read();

	wifi_ap_record_t ap;
	esp_wifi_sta_get_ap_info(&ap);

	sen.rssi = ap.rssi;

	return &sen;
}
