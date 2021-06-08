#include "print_partition_info.h"
#include "get_type_string.h"

#define HASH_LEN	32

//static const char* TAG = "PARTITION INFO";

void print_sha256 (const uint8_t *image_hash)
{
    char hash_print[HASH_LEN * 2 + 1];
    hash_print[HASH_LEN * 2] = 0;
    for (int i = 0; i < HASH_LEN; ++i) {
        sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
    }
    printf("SHA-256: %s\n", hash_print);
}

void print_partition_info(const esp_partition_t* partition){
	esp_ota_img_states_t state;

	printf("Addr: %08X\nLabel: %s\nSize: %uKB/%08X\nEncrypted: %s\nType: %s\nSubType: %s\n" ,
			partition->address,
			partition->label,
			partition->size / 1024, partition->size,
			partition->encrypted ? "yes" : "no",
			get_partition_type_string(partition->type),
			get_partition_subtype_string(partition->subtype));

	if(esp_ota_get_state_partition(partition, &state) == ESP_OK){
		get_ota_partition_state_string(state);
		printf("State: %s\n", get_ota_partition_state_string(state));
	} else {
		printf("State: error\n");
	}

	uint8_t sha_256[HASH_LEN] = { 0 };
	esp_partition_get_sha256(partition, sha_256);
	print_sha256(sha_256);
}

void print_ota_partition_description(const esp_app_desc_t* desc){
	printf("Project name: %s\nVersion: %s\nDate: %s\nTime: %s\nSecure Version: %u\nIdf Version: %s\n",
			desc->project_name,
			desc->version,
			desc->date,
			desc->time,
			desc->secure_version,
			desc->idf_ver);
}

