#include "esp_log.h"
#include "esp_wifi.h"
#include "debug_example.h"
#include "ir.h"
#include "print_ir_info.h"

static const char* TAG = "DEBUG EXAMPLE";

#define SCAN_PER_CHANNEL		10

void print_wifi_scan_short(uint8_t ch){
	uint16_t num_aps, n_num;
	static uint16_t num = 0;

	wifi_ap_record_t ap[SCAN_PER_CHANNEL];

	esp_wifi_scan_get_ap_num(&num_aps);
	n_num = SCAN_PER_CHANNEL;
	esp_wifi_scan_get_ap_records(&n_num, ap);
	ESP_LOGI(TAG, "Number APs found[%u]: %u", ch, num_aps);
	for(uint16_t j = 0; j < n_num; j++){
		ESP_LOGI(TAG, "%u: %u, %s, %d", ++num, ch, ap[j].ssid, ap[j].rssi);
	}
}

void ir_rx_callback(rmt_item32_t* item, size_t rx_size){
	ESP_LOGI(TAG, "Parsing data > rx_size: %u\n", rx_size);
	ir_protocol_t protocol = ir_decode(item, rx_size);
	print_ir_protocol(protocol);
	if(protocol != IR_UNKNOWN){
		ir_generic_t gen;
		ir_get_generic_code(&gen, item, rx_size);
		print_ir_generic_code(&gen);
		print_ir_generic_code_formated(&gen);
	}
}
