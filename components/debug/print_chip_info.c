
#include "esp_log.h"
#include "esp_system.h"

#include "debug_defs.h"
#include "get_type_string.h"

static const char *TAG = "CHIP INFO";

void print_idf_version(){
	ESP_LOGI(TAG, "ESP-IDF version: %s", esp_get_idf_version());
}

void print_chip_info(){
	esp_chip_info_t info;

	esp_chip_info(&info);
	ESP_LOGI(TAG, "Chip infod:"
			"\n\tcores: %u"
			"\n\tfeatures: 0x%08X"
			"\n\t\tEmb flash: %s"
			"\n\t\tWifi BNG: %s"
			"\n\t\tBLE: %s"
			"\n\t\tBT: %s"
			"\n\tmodel: %s"
			"\n\trevision: %u",
				info.cores,
				info.features,
				info.features & CHIP_FEATURE_EMB_FLASH ? "yes" : "no",
				info.features & CHIP_FEATURE_WIFI_BGN ? "yes" : "no",
				info.features & CHIP_FEATURE_BLE ? "yes" : "no",
				info.features & CHIP_FEATURE_BT ? "yes" : "no",
				get_chip_model_string(info.model),
				info.revision);
}

