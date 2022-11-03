#include "print_system_info.h"
//#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "debug_defs.h"
#include "get_type_string.h"

static const char* TAG = "SYSTEM INFO";

void print_system_reset_reason(){
	ESP_LOGI(TAG, "Reset reason: %s", get_system_reset_reason_string(esp_reset_reason()));
}

void print_base_mac_addr(){
	uint8_t mac[8];

	esp_base_mac_addr_get(mac);
	ESP_LOGI(TAG, "BASE MAC: "MAC_PRINT_STR,
			MAC_PRINT_ARG(mac));
}

//void print_system_algo(){
//	esp_mac_type_t
//}
