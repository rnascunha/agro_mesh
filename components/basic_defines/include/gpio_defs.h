#ifndef GPIO_DEFS_H__
#define GPIO_DEFS_H__

#include "driver/gpio.h"

#define GPIO_TYPE_T							gpio_num_t
#define GPIO_SET_DISABLE(pin)				gpio_set_direction(pin, GPIO_MODE_DISABLE)
#define GPIO_INIT(pin)						rtc_gpio_deinit(pin)
#define GPIO_NOT_CONNECTED					GPIO_NUM_NC

/**
 * Output options
 */
#define GPIO_SET_OUTPUT(pin)				gpio_set_direction(pin, GPIO_MODE_OUTPUT)
#define GPIO_SET_OUTPUT_OPEN_DRAIN(pin)		gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD)
//#define GPIO_SET_OUTPUT_PUSH_PULL(pin)

#define GPIO_LEVEL_HIGH(pin)				gpio_set_level(pin, 1)
#define GPIO_LEVEL_LOW(pin)					gpio_set_level(pin, 0)
#define GPIO_LEVEL(pin, level)				gpio_set_level(pin, level)

//#define GPIO_TOGGLE(pin)
/**
 * Input options
 */
#define GPIO_SET_INPUT(pin)					gpio_set_direction(pin, GPIO_MODE_INPUT); \
												gpio_set_pull_mode(pin, GPIO_FLOATING)
#define GPIO_SET_INPUT_PULLUP(pin)			gpio_set_direction(pin, GPIO_MODE_INPUT); \
												gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY)
#define GPIO_SET_INPUT_PULLDOWN(pin)		gpio_set_direction(pin, GPIO_MODE_INPUT); \
												gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY)

#define GPIO_READ_LEVEL(pin)				gpio_get_level(pin)

/**
 * Input Ouput
 */
#define GPIO_SET_INPUT_OUTPUT(pin)			gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT)
#define GPIO_SET_INPUT_OUTPUT_OPEN_DRAIN(pin)	gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT_OD)


#endif /* GPIO_DEFS_H__ */
