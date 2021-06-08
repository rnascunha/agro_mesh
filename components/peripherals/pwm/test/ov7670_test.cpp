#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "gpio.h"
#include "pwm.h"

static const char* TAG = "APP_MAIN";

#include "i2c_master.h"

#define	SIOC	GPIO_NUM_23
#define	SIOD	GPIO_NUM_25
#define	XCLK	GPIO_NUM_27
#define	VSYNC	GPIO_NUM_22
#define	HREF	GPIO_NUM_26
#define	PCLK	GPIO_NUM_21
#define	D0		GPIO_NUM_35
#define	D1		GPIO_NUM_17
#define	D2		GPIO_NUM_34
#define	D3		GPIO_NUM_5
#define	D6		GPIO_NUM_39
#define	D7		GPIO_NUM_18
#define	D8		GPIO_NUM_36
#define	D9		GPIO_NUM_19
#define	RESET	GPIO_NUM_15

#define XCLK_FREQ		20000000
#define XCLK_TIMER		LEDC_TIMER_0
#define XCLK_CHANNEL	LEDC_CHANNEL_0

#define REGISTER_PID	0x0A		//default 0x76
#define REGISTER_VER	0x0B		//default 0x70

#define SLAVE_ADDR		0x21

extern "C" void app_main(){
	GPIO_Basic reset_pin(RESET, GPIO_MODE_OUTPUT);

	PWM camera_clk(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_TIMER_0);
	camera_clk.init(XCLK_FREQ, XCLK, LEDC_TIMER_1_BIT, 1);

	I2C_Master camera_i2c(I2C_NUM_0, SIOC, SIOD, 100000);
	camera_i2c.init();

	ESP_LOGI(TAG, "Reseting CAM");
	reset_pin.write(1);
	vTaskDelay(10 / portTICK_RATE_MS);
	reset_pin.write(0);

	uint8_t data;
	int ret = camera_i2c.read(SLAVE_ADDR, REGISTER_PID, &data);
	if(ret >  0){
		ESP_LOGI(TAG, "I2C comm test ok 0x%02X", data);
	} else {
		ESP_LOGE(TAG, "I2C comm test ERROR %d", ret);
	}

	ret = camera_i2c.read(SLAVE_ADDR, REGISTER_VER, &data);
	if(ret >  0){
		ESP_LOGI(TAG, "I2C comm test ok 0x%02X", data);
	} else {
		ESP_LOGE(TAG, "I2C comm test ERROR %d", ret);
	}
}
