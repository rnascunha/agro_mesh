//#include "print_wifi_info.h"
#include "debug_defs.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "get_type_string.h"

static const char* TAG = "WIFI INFO";

void print_wifi_bandwidth(){
	wifi_bandwidth_t band;

	esp_wifi_get_bandwidth(WIFI_IF_STA, &band);
	ESP_LOGI(TAG, "Wifi Bandwitdth: %s", get_wifi_bandwidth_string(band));
}

void print_wifi_max_tx_power(){
	int8_t power;

	esp_wifi_get_max_tx_power(&power);
	ESP_LOGI(TAG, "Wifi TX power: %sdBm", get_wifi_max_tx_power_string(power));
}

void print_wifi_mac_addr(){
	uint8_t mac[8];

	esp_wifi_get_mac(WIFI_IF_STA, mac);
	ESP_LOGI(TAG, "WIFI MAC: "MAC_PRINT_STR, MAC_PRINT_ARG(mac));
}

void print_wifi_ap_records(){
	wifi_ap_record_t ap_record;

	esp_wifi_sta_get_ap_info(&ap_record);
	ESP_LOGI(TAG, "AP info:"
			"\n\tBSSID: "MAC6_PRINT_STR""
			"\n\tSSID: %s"
			"\n\tPri CH: %u"
			"\n\tSec CH: %s"
			"\n\tRSSI: %d"
			"\n\tAuthMode: %s"
			"\n\tPairwise cipher: %s"
			"\n\tGroup_cipher: %s"
			"\n\tAntena: %s"
			"\n\tPHY 11b: %s"
			"\n\tPHY 11g: %s"
			"\n\tPHY 11n: %s"
			"\n\tPHY LR: %s"
			"\n\tWPS support: %s"
			"\n\tCountry: %.*s"
			"\n\t\tMax TX power: %d"
			"\n\t\tStart CH: %u"
			"\n\t\tNum CH: %u"
			"\n\t\tPolicy: %s",
				MAC6_PRINT_ARG(ap_record.bssid), ap_record.ssid,
				ap_record.primary, get_wifi_second_channel_string(ap_record.second),
				ap_record.rssi, get_wifi_authmode_string(ap_record.authmode),
				get_wifi_cipher_type_string(ap_record.pairwise_cipher),
				get_wifi_cipher_type_string(ap_record.group_cipher), get_wifi_antena_string(ap_record.ant),
				ap_record.phy_11b ? "Yes" : "No", ap_record.phy_11g ? "Yes" : "No",
				ap_record.phy_11n ? "Yes" : "No",	ap_record.phy_lr ? "Yes" : "No",
				ap_record.wps ? "Yes" : "No",
				3, ap_record.country.cc, ap_record.country.max_tx_power, ap_record.country.schan,
				ap_record.country.nchan, get_wifi_contry_policy_string(ap_record.country.policy));
}

void print_wifi_country_info(){
	wifi_country_t country;

	esp_wifi_get_country(&country);
	ESP_LOGI(TAG, "Country: %.*s"
			"\n\t\tMax TX power: %d"
			"\n\t\tStart CH: %u"
			"\n\t\tNum CH: %u"
			"\n\t\tPolicy: %s",
				3, country.cc, country.max_tx_power, country.schan,
				country.nchan, get_wifi_contry_policy_string(country.policy));
}

void print_wifi_channel(){
	uint8_t pri_ch;
	wifi_second_chan_t sec_ch;

	esp_wifi_get_channel(&pri_ch, &sec_ch);
	ESP_LOGI(TAG, "Channel:"
			"\n\tPrimary: %u"
			"\n\tSecond: %s", pri_ch, get_wifi_second_channel_string(sec_ch));
}

