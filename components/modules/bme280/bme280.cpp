#include "bme280.h"
#include "bme280_defs.h"

#include "hardware_defs.h"
#include "macros/macro_basic.h"

#include <stddef.h>

static void parse_device_settings(const uint8_t *reg_data, BME280::settings *settings);
static void parse_sensor_data(const uint8_t *reg_data, BME280_defs::uncomp_data *uncomp_data);
static void compensate_data(uint8_t sensor_comp,
                              const BME280_defs::uncomp_data *uncomp_data,
                              BME280::data *comp_data, struct BME280::calib_data* calib_data);

uint8_t BME280::get_chip_id(){
	uint8_t chip_id = 0xFF;
	read_command(BME280_defs::CHIP_ID, &chip_id, 1);

	return chip_id;
}

int BME280::check_chip_id(){
	uint8_t chip_id = 0;
	read_command(BME280_defs::CHIP_ID, &chip_id, 1);

	return (chip_id != CHIP_ID) ? BME280_ERR_CHIP_ID : BME280_ERR_OK;
}

int BME280::soft_reset(){
    uint8_t try_run = 5, status_reg;

	uint8_t cmd = BME280_defs::POWER_ON_RESET;
	int ret = write_command(BME280_defs::RESET, &cmd, 1);
	if(ret < 0){
		return BME280_ERR_SOFT_RESET;
	}

    /* If NVM not copied yet, Wait for NVM to copy */
    do{
    	/**
    	 * Para funcionamento com SPI, o valor do delay teve que ser
    	 * modificado de 2 para 10 (no I2C, 2 funcionava perfeitamente)
    	 */
    	delay_ms(10);
    	ret = read_command(BME280_defs::STATUS, &status_reg, 1);
    }while ((ret > 0) && (try_run--) && (status_reg & BME280_defs::IM_UPDATE));

    if (status_reg & BME280_defs::IM_UPDATE){
        return BME280_ERR_SOFT_RESET_WAIT;
    }

	return BME280_ERR_OK;
}

int BME280::get_mode(Mode *mode){
    int ret;
    uint8_t data;

	ret = read_command(BME280_defs::PWR_CTRL, &data, 1);

	/* Assign the power mode in the device structure */
	*mode = (Mode)(data & 0x03);

    return ret;
}

int BME280::set_mode(Mode mode){
	int ret;
	Mode last_set_mode;

	ret = get_mode(&last_set_mode);

	/* If the sensor is not in sleep mode put the device to sleep
	* mode
	*/
	if ((ret > 0) && (last_set_mode != SLEEP)){
		ret = set_sleep_mode();
	}

	/* Set the power mode */
	if (ret > 0){
		ret = write_power_mode(mode);
	}

	return ret;
}

int BME280::config(settings *settings){
	int ret = set_osr_settings(settings);
	if(ret < 0){
		return BME280_ERR_CONFIG_SAMP;
	}

	ret = set_filter_standby_settings(settings);
	if(ret < 0){
		return BME280_ERR_CONFIG_FIL_STAND;
	}

	return BME280_ERR_OK;
}

int BME280::get_data(uint8_t sensor_comp, data *comp_data){
	int ret = BME_ERR_GET_DATA;

	/* Array to store the pressure, temperature and humidity data read from
	* the sensor
	*/
	uint8_t reg_data[BME280_defs::P_T_H_DATA_LEN] = {};
	BME280_defs::uncomp_data uncomp_data = {};

	if (comp_data != NULL){
		/* Read the pressure and temperature data from the sensor */
		ret = read_command(BME280_defs::DATA, reg_data, BME280_defs::P_T_H_DATA_LEN);
		if (ret > 0){
			/* Parse the read data from the sensor */
			parse_sensor_data(reg_data, &uncomp_data);

			/* Compensate the pressure and/or temperature and/or
			* humidity data from the sensor
			*/
			compensate_data(sensor_comp, &uncomp_data, comp_data, &calib_data);
		}
	}

	return ret;
}

/**
 *
 */
int BME280::get_calib_data(){
	int ret;
	/* Array to store calibration data */
	uint8_t calib_data[BME280_defs::TEMP_PRESS_CALIB_DATA_LEN] = { 0 };

	/* Read the calibration data from the sensor */
	ret = read_command(BME280_defs::TEMP_PRESS_CALIB_DATA, calib_data, BME280_defs::TEMP_PRESS_CALIB_DATA_LEN);
	if (ret < 0){
		return BME280_ERR_CALIB_DATA;
	}

	/* Parse temperature and pressure calibration data and store
	* it in device structure
	*/
	this->calib_data.dig_T1 = CONCAT_BYTES(calib_data[1], calib_data[0]);
	this->calib_data.dig_T2 = (int16_t)CONCAT_BYTES(calib_data[3], calib_data[2]);
	this->calib_data.dig_T3 = (int16_t)CONCAT_BYTES(calib_data[5], calib_data[4]);
	this->calib_data.dig_P1 = CONCAT_BYTES(calib_data[7], calib_data[6]);
	this->calib_data.dig_P2 = (int16_t)CONCAT_BYTES(calib_data[9], calib_data[8]);
	this->calib_data.dig_P3 = (int16_t)CONCAT_BYTES(calib_data[11], calib_data[10]);
	this->calib_data.dig_P4 = (int16_t)CONCAT_BYTES(calib_data[13], calib_data[12]);
	this->calib_data.dig_P5 = (int16_t)CONCAT_BYTES(calib_data[15], calib_data[14]);
	this->calib_data.dig_P6 = (int16_t)CONCAT_BYTES(calib_data[17], calib_data[16]);
	this->calib_data.dig_P7 = (int16_t)CONCAT_BYTES(calib_data[19], calib_data[18]);
	this->calib_data.dig_P8 = (int16_t)CONCAT_BYTES(calib_data[21], calib_data[20]);
	this->calib_data.dig_P9 = (int16_t)CONCAT_BYTES(calib_data[23], calib_data[22]);
	this->calib_data.dig_H1 = calib_data[25];

	/* Read the humidity calibration data from the sensor */
	ret = read_command(BME280_defs::HUMIDITY_CALIB_DATA, calib_data, BME280_defs::HUMIDITY_CALIB_DATA_LEN);
	if (ret < 0){
		return BME280_ERR_CALIB_DATA;
	}

	/* Parse humidity calibration data and store it in
	 * device structure
	 */
	int16_t dig_H4_lsb;
	int16_t dig_H4_msb;
	int16_t dig_H5_lsb;
	int16_t dig_H5_msb;

	this->calib_data.dig_H2 = (int16_t)CONCAT_BYTES(calib_data[1], calib_data[0]);
	this->calib_data.dig_H3 = calib_data[2];
	dig_H4_msb = (int16_t)(int8_t)calib_data[3] * 16;
	dig_H4_lsb = (int16_t)(calib_data[4] & 0x0F);
	this->calib_data.dig_H4 = dig_H4_msb | dig_H4_lsb;
	dig_H5_msb = (int16_t)(int8_t)calib_data[5] * 16;
	dig_H5_lsb = (int16_t)(calib_data[4] >> 4);
	this->calib_data.dig_H5 = dig_H5_msb | dig_H5_lsb;
	this->calib_data.dig_H6 = (int8_t)calib_data[6];

    return ret;
}

int BME280::write_power_mode(uint8_t sensor_mode){
    int ret;

    /* Variable to store the value read from power mode register */
    uint8_t sensor_mode_reg_val;

    /* Read the power mode register */
    ret = read_command(BME280_defs::PWR_CTRL, &sensor_mode_reg_val, 1);

    /* Set the power mode */
    if (ret > 0){
        sensor_mode_reg_val = BME280_SET_BITS_POS_0(sensor_mode_reg_val, BME280_SENSOR_MODE, sensor_mode);

        /* Write the power mode in the register */
        ret = write_command(BME280_defs::PWR_CTRL, &sensor_mode_reg_val, 1);
    }

    return ret;
}

int BME280::set_sleep_mode(){
    int ret;
    uint8_t reg_data[4];
    settings settings;

	ret = read_command(BME280_defs::CTRL_HUM, reg_data, 4);
	if (ret > 0){
		parse_device_settings(reg_data, &settings);
		ret = soft_reset();
		if (ret > 0){
			ret = reload_device_settings(&settings);
		}
	}

	return ret;
}

int BME280::set_osr_humidity_settings(const settings *settings){
    int8_t ret;
    uint8_t ctrl_hum;
    uint8_t ctrl_meas;

    ctrl_hum = settings->osr_h & BME280_CTRL_HUM_MSK;

    /* Write the humidity control value in the register */
    ret = read_command(BME280_defs::CTRL_HUM, &ctrl_hum, 1);

    /* Humidity related changes will be only effective after a
     * write operation to ctrl_meas register
     */
    if (ret > 0){
        ret = read_command(BME280_defs::CTRL_MEAS, &ctrl_meas, 1);
        if (ret > 0){
            ret = write_command(BME280_defs::CTRL_MEAS, &ctrl_meas, 1);
        }
    }

    return ret;
}

int BME280::set_filter_standby_settings(const settings *settings){
    int ret;
    uint8_t reg_data;

    ret = read_command(BME280_defs::CONFIG, &reg_data, 1);
    if (ret > 0)
    {
        if (settings->desired_set & BME280_FILTER_SEL){
        	reg_data = BME280_SET_BITS(reg_data, BME280_FILTER, settings->filter);
        }
        if (settings->desired_set & BME280_STANDBY_SEL){
        	reg_data = BME280_SET_BITS(reg_data, BME280_STANDBY, settings->standby_time);
        }

        /* Write the oversampling settings in the register */
        ret = write_command(BME280_defs::CONFIG, &reg_data, 1);
    }

    return ret;
}

int BME280::set_osr_press_temp_settings(const settings *settings){
	int ret;
	uint8_t reg_data;

	ret = read_command(BME280_defs::CTRL_MEAS, &reg_data, 1);
	if (ret > 0){
		if (settings->desired_set & BME280_OSR_PRESS_SEL){
			reg_data = BME280_SET_BITS(reg_data, BME280_CTRL_PRESS, settings->osr_p);
		}
		if (settings->desired_set & BME280_OSR_TEMP_SEL){
			reg_data = BME280_SET_BITS(reg_data, BME280_CTRL_TEMP, settings->osr_t);
		}

		/* Write the oversampling settings in the register */
		ret = write_command(BME280_defs::CTRL_MEAS, &reg_data, 1);
	}
	return ret;
}

int BME280::set_osr_settings(const settings *settings){
	int ret = BME280_ERR_INVALID_OSR;

	if (settings->desired_set & BME280_OSR_HUM_SEL){
		ret = set_osr_humidity_settings(settings);
	}
	if (settings->desired_set & (BME280_OSR_TEMP_SEL | BME280_OSR_PRESS_SEL)){
		ret = set_osr_press_temp_settings(settings);
	}

	return ret;
}

int BME280::reload_device_settings(const settings *settings){
	int ret = set_osr_settings(settings);

	if (ret > 0){
		ret = set_filter_standby_settings(settings);
	}

	return ret;
}

static void parse_device_settings(const uint8_t *reg_data, BME280::settings *settings){
    settings->osr_h = (BME280::Sampling)BME280_GET_BITS_POS_0(reg_data[0], BME280_CTRL_HUM);
    settings->osr_p = (BME280::Sampling)BME280_GET_BITS(reg_data[2], BME280_CTRL_PRESS);
    settings->osr_t = (BME280::Sampling)BME280_GET_BITS(reg_data[2], BME280_CTRL_TEMP);
    settings->filter = (BME280::IIR_Filter_Coeficient)BME280_GET_BITS(reg_data[3], BME280_FILTER);
    settings->standby_time = (BME280::Stand_By_Time_ms)BME280_GET_BITS(reg_data[3], BME280_STANDBY);
}

static void parse_sensor_data(const uint8_t *reg_data, BME280_defs::uncomp_data *uncomp_data){
	/* Variables to store the sensor data */
	uint32_t data_xlsb;
	uint32_t data_lsb;
	uint32_t data_msb;

	/* Store the parsed register values for pressure data */
	data_msb = (uint32_t)reg_data[0] << 12;
	data_lsb = (uint32_t)reg_data[1] << 4;
	data_xlsb = (uint32_t)reg_data[2] >> 4;
	uncomp_data->pressure = data_msb | data_lsb | data_xlsb;

	/* Store the parsed register values for temperature data */
	data_msb = (uint32_t)reg_data[3] << 12;
	data_lsb = (uint32_t)reg_data[4] << 4;
	data_xlsb = (uint32_t)reg_data[5] >> 4;
	uncomp_data->temperature = data_msb | data_lsb | data_xlsb;

	/* Store the parsed register values for temperature data */
	data_lsb = (uint32_t)reg_data[6] << 8;
	data_msb = (uint32_t)reg_data[7];
	uncomp_data->humidity = data_msb | data_lsb;
}

#ifdef BME280_FLOAT_ENABLE
static double compensate_temperature(const struct BME280_defs::uncomp_data *uncomp_data,
		struct BME280::calib_data *calib_data){
    double var1;
    double var2;
    double temperature;
    double temperature_min = -40;
    double temperature_max = 85;

    var1 = ((double)uncomp_data->temperature) / 16384.0 - ((double)calib_data->dig_T1) / 1024.0;
    var1 = var1 * ((double)calib_data->dig_T2);
    var2 = (((double)uncomp_data->temperature) / 131072.0 - ((double)calib_data->dig_T1) / 8192.0);
    var2 = (var2 * var2) * ((double)calib_data->dig_T3);
    calib_data->t_fine = (int32_t)(var1 + var2);
    temperature = (var1 + var2) / 5120.0;
    if (temperature < temperature_min){
        temperature = temperature_min;
    }else if (temperature > temperature_max){
        temperature = temperature_max;
    }

    return temperature;
}

static double compensate_pressure(const struct BME280_defs::uncomp_data *uncomp_data,
                                  const struct BME280::calib_data *calib_data){
    double var1;
    double var2;
    double var3;
    double pressure;
    double pressure_min = 30000.0;
    double pressure_max = 110000.0;

    var1 = ((double)calib_data->t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)calib_data->dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)calib_data->dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)calib_data->dig_P4) * 65536.0);
    var3 = ((double)calib_data->dig_P3) * var1 * var1 / 524288.0;
    var1 = (var3 + ((double)calib_data->dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)calib_data->dig_P1);

    /* avoid exception caused by division by zero */
    if (var1){
        pressure = 1048576.0 - (double) uncomp_data->pressure;
        pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;
        var1 = ((double)calib_data->dig_P9) * pressure * pressure / 2147483648.0;
        var2 = pressure * ((double)calib_data->dig_P8) / 32768.0;
        pressure = pressure + (var1 + var2 + ((double)calib_data->dig_P7)) / 16.0;
        if (pressure < pressure_min){
            pressure = pressure_min;
        }else if (pressure > pressure_max){
            pressure = pressure_max;
        }
    }else{ /* Invalid case */
        pressure = pressure_min;
    }

    return pressure;
}

static double compensate_humidity(const struct BME280_defs::uncomp_data *uncomp_data,
                                  const struct BME280::calib_data *calib_data){
    double humidity;
    double humidity_min = 0.0;
    double humidity_max = 100.0;
    double var1;
    double var2;
    double var3;
    double var4;
    double var5;
    double var6;

    var1 = ((double)calib_data->t_fine) - 76800.0;
    var2 = (((double)calib_data->dig_H4) * 64.0 + (((double)calib_data->dig_H5) / 16384.0) * var1);
    var3 = uncomp_data->humidity - var2;
    var4 = ((double)calib_data->dig_H2) / 65536.0;
    var5 = (1.0 + (((double)calib_data->dig_H3) / 67108864.0) * var1);
    var6 = 1.0 + (((double)calib_data->dig_H6) / 67108864.0) * var1 * var5;
    var6 = var3 * var4 * (var5 * var6);
    humidity = var6 * (1.0 - ((double)calib_data->dig_H1) * var6 / 524288.0);
    if (humidity > humidity_max){
        humidity = humidity_max;
    }else if (humidity < humidity_min){
        humidity = humidity_min;
    }

    return humidity;
}

#else /* BME280_FLOAT_ENABLE */

static int32_t compensate_temperature(const struct BME280_defs::uncomp_data *uncomp_data,
                                      struct BME280::calib_data *calib_data){
    int32_t var1;
    int32_t var2;
    int32_t temperature;
    int32_t temperature_min = -4000;
    int32_t temperature_max = 8500;

    var1 = (int32_t)((uncomp_data->temperature / 8) - ((int32_t)calib_data->dig_T1 * 2));
    var1 = (var1 * ((int32_t)calib_data->dig_T2)) / 2048;
    var2 = (int32_t)((uncomp_data->temperature / 16) - ((int32_t)calib_data->dig_T1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)calib_data->dig_T3)) / 16384;
    calib_data->t_fine = var1 + var2;
    temperature = (calib_data->t_fine * 5 + 128) / 256;
    if (temperature < temperature_min){
        temperature = temperature_min;
    } else if (temperature > temperature_max){
        temperature = temperature_max;
    }

    return temperature;
}

#if BME280_64BIT_ENABLE
static uint32_t compensate_pressure(const struct BME280_defs::uncomp_data *uncomp_data,
                                    const struct BME280::calib_data *calib_data){
    int64_t var1;
    int64_t var2;
    int64_t var3;
    int64_t var4;
    uint32_t pressure;
    uint32_t pressure_min = 3000000;
    uint32_t pressure_max = 11000000;

    var1 = ((int64_t)calib_data->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)calib_data->dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib_data->dig_P5) * 131072);
    var2 = var2 + (((int64_t)calib_data->dig_P4) * 34359738368);
    var1 = ((var1 * var1 * (int64_t)calib_data->dig_P3) / 256) + ((var1 * ((int64_t)calib_data->dig_P2) * 4096));
    var3 = ((int64_t)1) * 140737488355328;
    var1 = (var3 + var1) * ((int64_t)calib_data->dig_P1) / 8589934592;

    /* To avoid divide by zero exception */
    if (var1 != 0){
        var4 = 1048576 - uncomp_data->pressure;
        var4 = (((var4 * INT64_C(2147483648)) - var2) * 3125) / var1;
        var1 = (((int64_t)calib_data->dig_P9) * (var4 / 8192) * (var4 / 8192)) / 33554432;
        var2 = (((int64_t)calib_data->dig_P8) * var4) / 524288;
        var4 = ((var4 + var1 + var2) / 256) + (((int64_t)calib_data->dig_P7) * 16);
        pressure = (uint32_t)(((var4 / 2) * 100) / 128);
        if (pressure < pressure_min){
            pressure = pressure_min;
        }else if (pressure > pressure_max){
            pressure = pressure_max;
        }
    }
    else{
        pressure = pressure_min;
    }

    return pressure;
}
#else /* BME280_64BIT_ENABLE */
static uint32_t compensate_pressure(const struct BME280_defs::uncomp_data *uncomp_data,
                                    const struct BME280::calib_data *calib_data){
	int32_t var1;
	int32_t var2;
	int32_t var3;
	int32_t var4;
	uint32_t var5;
	uint32_t pressure;
	uint32_t pressure_min = 30000;
	uint32_t pressure_max = 110000;

	var1 = (((int32_t)calib_data->t_fine) / 2) - (int32_t)64000;
	var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t)calib_data->dig_P6);
	var2 = var2 + ((var1 * ((int32_t)calib_data->dig_P5)) * 2);
	var2 = (var2 / 4) + (((int32_t)calib_data->dig_P4) * 65536);
	var3 = (calib_data->dig_P3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8;
	var4 = (((int32_t)calib_data->dig_P2) * var1) / 2;
	var1 = (var3 + var4) / 262144;
	var1 = (((32768 + var1)) * ((int32_t)calib_data->dig_P1)) / 32768;

	/* avoid exception caused by division by zero */
	if (var1){
		var5 = (uint32_t)((uint32_t)1048576) - uncomp_data->pressure;
		pressure = ((uint32_t)(var5 - (uint32_t)(var2 / 4096))) * 3125;
		if (pressure < 0x80000000){
			pressure = (pressure << 1) / ((uint32_t)var1);
		}else{
			pressure = (pressure / (uint32_t)var1) * 2;
		}
		var1 = (((int32_t)calib_data->dig_P9) * ((int32_t)(((pressure / 8) * (pressure / 8)) / 8192))) / 4096;
		var2 = (((int32_t)(pressure / 4)) * ((int32_t)calib_data->dig_P8)) / 8192;
		pressure = (uint32_t)((int32_t)pressure + ((var1 + var2 + calib_data->dig_P7) / 16));
		if (pressure < pressure_min){
			pressure = pressure_min;
		}else if (pressure > pressure_max){
			pressure = pressure_max;
		}
	}else{
		pressure = pressure_min;
	}

	return pressure;
}
#endif /* BME280_64BIT_ENABLE */

static uint32_t compensate_humidity(const struct BME280_defs::uncomp_data *uncomp_data,
                                    const struct BME280::calib_data *calib_data){
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    uint32_t humidity;
    uint32_t humidity_max = 102400;

    var1 = calib_data->t_fine - ((int32_t)76800);
    var2 = (int32_t)(uncomp_data->humidity * 16384);
    var3 = (int32_t)(((int32_t)calib_data->dig_H4) * 1048576);
    var4 = ((int32_t)calib_data->dig_H5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2 = (var1 * ((int32_t)calib_data->dig_H6)) / 1024;
    var3 = (var1 * ((int32_t)calib_data->dig_H3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2 = ((var4 * ((int32_t)calib_data->dig_H2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t)calib_data->dig_H1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    humidity = (uint32_t)(var5 / 4096);
    if (humidity > humidity_max){
        humidity = humidity_max;
    }

    return humidity;
}
#endif /* BME280_FLOAT_ENABLE */

static void compensate_data(uint8_t sensor_comp,
                              const BME280_defs::uncomp_data *uncomp_data,
                              BME280::data *comp_data, struct BME280::calib_data* calib_data){

    if ((uncomp_data != NULL) && (comp_data != NULL) && (calib_data != NULL)){
        /* Initialize to zero */
        comp_data->temperature = 0;
        comp_data->pressure = 0;
        comp_data->humidity = 0;

        /* If pressure or temperature component is selected */
        if (sensor_comp & (BME280_PRESS | BME280_TEMP | BME280_HUM)){
            /* Compensate the temperature data */
            comp_data->temperature = compensate_temperature(uncomp_data, calib_data);
        }
        if (sensor_comp & BME280_PRESS){
            /* Compensate the pressure data */
            comp_data->pressure = compensate_pressure(uncomp_data, calib_data);
        }
        if (sensor_comp & BME280_HUM){
            /* Compensate the humidity data */
            comp_data->humidity = compensate_humidity(uncomp_data, calib_data);
        }
    }
}
