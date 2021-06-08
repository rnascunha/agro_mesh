#ifndef PRINT_PARTITION_INFO_H__
#define PRINT_PARTITION_INFO_H__

#include <stdint.h>
#include "esp_partition.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"
#include "esp_image_format.h"

#ifdef CONFIG_PRINT_PARTITION_INFO_LOG_LEVEL

void print_sha256 (const uint8_t *image_hash);
void print_partition_info(const esp_partition_t* partition);
void print_ota_partition_description(const esp_app_desc_t* desc);

#else

inline void __attribute__((always_inline)) print_sha256 (const uint8_t *image_hash){}
inline void __attribute__((always_inline)) print_partition_info(const esp_partition_t* partition){}
inline void __attribute__((always_inline)) print_ota_partition_description(const esp_app_desc_t* desc){}

#endif /* CONFIG_PRINT_PARTITION_INFO_LOG_LEVEL */

#ifdef __cplusplus
}
#endif

#endif /* PRINT_PARTITION_INFO_H__ */
