#include "driver/gpio.h"
#include "i2c_master.h"

#include "esp_log.h"

static const char* TAG = "APP_MAIN";

#define	SCL_PIN			GPIO_NUM_25
#define	SDA_PIN			GPIO_NUM_26

#define I2C_FREQ_HZ		100000

extern "C" void app_main(){
	I2C_Master i2c(I2C_NUM_0, SCL_PIN, SDA_PIN, I2C_FREQ_HZ);
	i2c.init();

	uint8_t addr = 0;
	while(addr != 0xFF){
		addr = i2c.probe_addr(addr);
		if(addr != 0xFF){
			ESP_LOGI(TAG, "Device found at addr 0x%02X", addr);
			addr++;
		} else {
			ESP_LOGI(TAG, "Device not found");
		}
	}
	return;
}
