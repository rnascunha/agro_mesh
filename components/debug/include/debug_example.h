#ifndef DEBUG_EXAMPLE_H__
#define DEBUG_EXAMPLE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "driver/rmt.h"

void print_wifi_scan_short(uint8_t ch);
void ir_rx_callback(rmt_item32_t* item, size_t rx_size);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_EXAMPLE_H__ */
