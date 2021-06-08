#include "bme280_i2c.h"
#include "bme280_defs.h"

BME280_I2C::BME280_I2C(I2C_Master* i2c) :
	i2c(i2c), slave_addr(SLAVE_ADDR_BASE){}

int BME280_I2C::init(){
	int ret = check_chip_id();

	if(ret == BME280_ERR_CHIP_ID){
		slave_addr |= 0x01;
		ret = check_chip_id();
		if(ret == BME280_ERR_CHIP_ID){
			slave_addr = SLAVE_ADDR_BASE;
			return BME280_ERR_INIT_ADDR;
		}
	}

	ret = soft_reset();
	if(ret < 0){
		return ret;
	}

	ret = get_calib_data();
	if(ret < 0){
		return ret;
	}

	return (int)slave_addr;
}

int BME280_I2C::init(bool sdo){
	slave_addr |= sdo;

	int ret = check_chip_id();
	if(ret == BME280_ERR_CHIP_ID){
		slave_addr = SLAVE_ADDR_BASE;
		return BME280_ERR_INIT_ADDR;
	}

	ret = soft_reset();
	if(ret < 0){
		return ret;
	}

	ret = get_calib_data();
	if(ret < 0){
		return ret;
	}

	return (int)slave_addr;
}

/**
 *
 */
int BME280_I2C::read_command(uint8_t cmd, uint8_t* aws, unsigned int length){
	return i2c->read(slave_addr, cmd, aws, length, true);
}


int BME280_I2C::write_command(uint8_t cmd, uint8_t* data, unsigned int length){
	return i2c->write(slave_addr, cmd, data, length, true);
}
