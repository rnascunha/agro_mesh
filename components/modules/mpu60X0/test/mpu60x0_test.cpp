#include "driver/gpio.h"
#include "i2c_master.h"

#include "hardware_defs.h"
#include "mpu60x0.h"

#include <stdio.h>
#include <stdint.h>

#include "esp_log.h"

#include "macros/macro_basic.h"

static const char* TAG = "APP_MAIN";

#define	SCL_PIN			GPIO_NUM_25
#define	SDA_PIN			GPIO_NUM_26

#define INTERRUPT_PIN	GPIO_NUM_14

#define I2C_FREQ_HZ		400000

#define CONFIG_USE_FIFO_OPERATIONS
#define CONFIG_USE_FLOAT_AXIS

extern "C" void app_main(){
	I2C_Master i2c(I2C_NUM_0, SCL_PIN, SDA_PIN, I2C_FREQ_HZ);
	i2c.init();

	MPU60X0 sensor(&i2c);

	int ret = sensor.init();
	if(ret > 0){
		ESP_LOGI(TAG, "Sensor found addr 0x%02X", ret);
	} else {
		ESP_LOGI(TAG, "Sensor NOT found %d", ret);
		return;
	}

	uint8_t id = sensor.who_am_i();
	if(id == MPU60X0::chip_id){
		ESP_LOGI(TAG, "Sensor id check 0x%02X", id);
	} else {
		ESP_LOGI(TAG, "Sensor NOT found 0x%02X", id);
		return;
	}

	MPU60X0::settings s;
	s.acc = MPU60X0::ACC_2G;
	s.gyro = MPU60X0::GYRO_2000;
//	s.pwr_mgmt_2 = MPU60X0::Stand_By_Acc_X | MPU60X0::Stand_By_Acc_Z | MPU60X0::Stand_By_Gyro_Y;
//	s.pwr_mgmt_1 = MPU60X0::Cycle;
//	s.pwr_mgmt_2 = MPU60X0::LP_Wake_1_25Hz;
	s.config = MPU60X0::Low_Pass_5Hz;
	s.sample_rate_div = 255;	//Ver registrador 25

#ifdef CONFIG_USE_FIFO_OPERATIONS
	s.fifo_en = MPU60X0::acc_en | MPU60X0::gyro_en | MPU60X0::temp_en;
//	s.fifo_en = MPU60X0::acc_en | MPU60X0::temp_en;
#endif /* CONFIG_USE_FIFO_OPERATIONS */

	ret = sensor.config(&s);
	if(ret < 0){
		ESP_LOGI(TAG, "Sensor config error");
	}

    /* Continuously stream sensor data */
    while (true) {
#ifdef CONFIG_USE_FIFO_OPERATIONS
    	uint8_t data[MPU60X0_BUFFER_SIZE];

		ret = sensor.read_fifo_data(data, MPU60X0_BUFFER_SIZE);
		if(ret < 0){
			ESP_LOGI(TAG, "Error reading FIFO %d", ret);
		} else {
			ESP_LOGI(TAG, ">>> Read %d datas from fifo", ret);

			unsigned int t = 0;
#ifndef CONFIG_USE_FLOAT_AXIS
    		while(t < ret){
    			int16_t temp;
    			MPU60X0::Axis acc_data, gyro_data;

    			t += MPU60X0::get_fifo_raw_data(&s, data + t, ret, &temp, &acc_data, &gyro_data);

    			ESP_LOGI(TAG, "Temp: %.3f", MPU60X0::convert_temp_data(temp));
        		ESP_LOGI(TAG, "Acc: x=%.3f y=%.3f z=%.3f", MPU60X0::convert_acc_data(&s, acc_data.x),
        													MPU60X0::convert_acc_data(&s, acc_data.y),
    														MPU60X0::convert_acc_data(&s, acc_data.z));
        		ESP_LOGI(TAG, "Gyro: x=%.3f y=%.3f z=%.3f", MPU60X0::convert_gyro_data(&s, gyro_data.x),
    														MPU60X0::convert_gyro_data(&s, gyro_data.y),
    														MPU60X0::convert_gyro_data(&s, gyro_data.z));
    		}
#else /* CONFIG_USE_FLOAT_AXIS */
    		while(t < ret){
    			MPU60X0_FLOAT_TYPE temp;
				MPU60X0::Axis_Float acc_data, gyro_data;

				t += MPU60X0::get_fifo_data(&s, data + t, ret, &temp, &acc_data, &gyro_data);

				ESP_LOGI(TAG, "Temp: %.3f", temp);
				ESP_LOGI(TAG, "Acc: x=%.3f y=%.3f z=%.3f", acc_data.x, acc_data.y, acc_data.z);
				ESP_LOGI(TAG, "Gyro: x=%.3f y=%.3f z=%.3f", gyro_data.x, gyro_data.y, gyro_data.z);
			}
#endif /* CONFIG_USE_FLOAT_AXIS */
		}
#else /* CONFIG_USE_FIFO_OPERATIONS */
#ifndef CONFIG_USE_FLOAT_AXIS
		int16_t temp;
		ret = sensor.get_temp_raw_data(&temp);
		if(ret > 0){
			ESP_LOGI(TAG, "Temp: %.3f", MPU60X0::convert_temp_data(temp));
		} else {
			ESP_LOGI(TAG, "Error reading temp");
		}

    	MPU60X0::Axis data;
    	ret = sensor.get_acc_raw_data(&data);
    	if(ret > 0){
    		ESP_LOGI(TAG, "Acc: x=%.3f y=%.3f z=%.3f", MPU60X0::convert_acc_data(&s, data.x),
    													MPU60X0::convert_acc_data(&s, data.y),
														MPU60X0::convert_acc_data(&s, data.z));
    	} else {
    		ESP_LOGI(TAG, "Error reading ACC");
    	}

    	ret = sensor.get_gyro_raw_data(&data);
    	if(ret > 0){
    		ESP_LOGI(TAG, "Gyro: x=%.3f y=%.3f z=%.3f", MPU60X0::convert_gyro_data(&s, data.x),
														MPU60X0::convert_gyro_data(&s, data.y),
														MPU60X0::convert_gyro_data(&s, data.z));
    	} else {
    		ESP_LOGI(TAG, "Error reading GYRO");
    	}
#else /* CONFIG_USE_FLOAT_AXIS */
		MPU60X0_FLOAT_TYPE temp;
		ret = sensor.get_temp(&temp);
		if(ret > 0){
			ESP_LOGI(TAG, "Temp: %.3f", temp);
		} else {
			ESP_LOGI(TAG, "Error reading temp");
		}

		MPU60X0::Axis_Float data;
		ret = sensor.get_acc_data(s.acc, &data);
		if(ret > 0){
			ESP_LOGI(TAG, "Acc: x=%.3f y=%.3f z=%.3f", data.x, data.y, data.z);
		} else {
			ESP_LOGI(TAG, "Error reading ACC");
		}

		ret = sensor.get_gyro_data(s.gyro, &data);
		if(ret > 0){
			ESP_LOGI(TAG, "Gyro: x=%.3f y=%.3f z=%.3f", data.x, data.y, data.z);
		} else {
			ESP_LOGI(TAG, "Error reading GYRO");
		}
#endif /* CONFIG_USE_FLOAT_AXIS */
#endif /* CONFIG_USE_FIFO_OPERATIONS */

    	delay_ms(1000);
    }

}

