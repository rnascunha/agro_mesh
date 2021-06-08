#include "print_ir_info.h"
#include "nec_defs.h"
#include "esp_log.h"
#include "get_type_string.h"
#include "ir_defs.h"

static const char* TAG = "IR INFO";

void print_rmt_item32(rmt_item32_t* item, size_t size){

	while(size--){
		ESP_LOGI(TAG, "> %u: %u|%u: %u",
				item->level0, RMT_ITEM_DURATION(item->duration0),
				item->level1, RMT_ITEM_DURATION(item->duration1));
		item++;
	}
}

void print_rmt_item32_ticks(rmt_item32_t* item, size_t size){
	while(size--){
		ESP_LOGI(TAG, "> %u: %u|%u: %u",
				item->level0, item->duration0,
				item->level1, item->duration1);
		item++;
	}
}

void print_ir_protocol(ir_protocol_t protocol){
	ESP_LOGI(TAG, "IR protocol: %s", get_ir_protocol_string(protocol));
}

void print_ir_generic_code(ir_generic_t* gen){
	ESP_LOGI(TAG, "Print gen[%u]", gen->size);

	uint32_t level = 1;
	for(size_t i  = 0; i < gen->size; i++){
		printf("%03u > %u: %u\n", i, level, gen->data[i]);
		level = !level;
	}
}

void print_ir_generic_code_formated(ir_generic_t* gen){
	ESP_LOGI(TAG, "Print gen formated[%u]", gen->size);

	for(size_t i  = 0; i < gen->size; i++){
		printf("%u,", gen->data[i]);
	}
	printf("\n");
}
