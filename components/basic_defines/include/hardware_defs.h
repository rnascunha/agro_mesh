#ifndef __HARDWARE_DEFS_ESP32_H__
#define __HARDWARE_DEFS_ESP32_H__

#include "esp32/rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#define disable_interrupts()	portDISABLE_INTERRUPTS()
#define enable_interrupts()		portENABLE_INTERRUPTS()

//#define disable_interrupts()
//#define enable_interrupts()

//#define delay_ms(x)				vTaskDelay((x) / portTICK_RATE_MS)
#define delay_ms(x)				vTaskDelay((x) / portTICK_PERIOD_MS)
#define delay_us(x)				ets_delay_us(x)

//#define PROGMEM
//#define pgm_read_byte

#endif /* __HARDWARE_DEFS_ESP32_H__ */
