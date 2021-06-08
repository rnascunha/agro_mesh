#include "mpu60x0.h"
#include "mpu60x0_defs.h"

#include "macros/macro_basic.h"

#define MPU60X0_SLAVE_ADDR_BASE		0b1101000

#ifdef CONFIG_GRAVITY_EARTH
static const MPU60X0_FLOAT_TYPE GRAVITY_EARTH = CONFIG_GRAVITY_EARTH;
#else
static const MPU60X0_FLOAT_TYPE GRAVITY_EARTH = 9.80665f;
#endif /* CONFIG_GRAVITY_EARTH */

MPU60X0::MPU60X0(I2C_Master *i2c) :
	i2c(i2c), slave_addr(MPU60X0_SLAVE_ADDR_BASE){}

int MPU60X0::init(){
	if(check_chip_id() > 0){
		return slave_addr;
	}

	slave_addr |= 0x01;
	if(check_chip_id() > 0){
		return slave_addr;
	}

	slave_addr = MPU60X0_SLAVE_ADDR_BASE;
	return MPU60X0_ERR_DEV_NOT_FOUND;
}

int MPU60X0::init(bool ad0){
	slave_addr |= ad0;

	if(check_chip_id() > 0){
		return slave_addr;
	}

	slave_addr = MPU60X0_SLAVE_ADDR_BASE;
	return MPU60X0_ERR_DEV_NOT_FOUND;
}

int MPU60X0::config(settings* set){
	int ret = write_command(MPU60X0_defs::PWR_MGMT_2, (uint8_t*)&set->pwr_mgmt_2, 1);
		if(ret < 0){
			return MPU60X0_ERR_CONFIG_ERROR;
	}

	ret = write_command(MPU60X0_defs::PWR_MGMT_1, (uint8_t*)&set->pwr_mgmt_1, 1);
	if(ret < 0){
		return MPU60X0_ERR_CONFIG_ERROR;
	}

	ret = write_command(MPU60X0_defs::CONFIG, (uint8_t*)&set->config, 1);
	if(ret < 0){
		return MPU60X0_ERR_CONFIG_ERROR;
	}

	ret = write_command(MPU60X0_defs::FIFO_EN, (uint8_t*)&set->fifo_en, 1);
	if(ret < 0){
		return MPU60X0_ERR_CONFIG_ERROR;
	}

	if(set->fifo_en){
		uint8_t data = 0b01000100;		//Enable and reset fifo
		ret = write_command(MPU60X0_defs::USER_CTRL, &data, 1);
		if(ret < 0){
			return MPU60X0_ERR_CONFIG_ERROR;
		}
	}

	ret = write_command(MPU60X0_defs::SMPRT_DIV, (uint8_t*)&set->sample_rate_div, 1);
	if(ret < 0){
		return MPU60X0_ERR_CONFIG_ERROR;
	}

	ret = write_command(MPU60X0_defs::ACCEL_CONFIG, (uint8_t*)&set->acc, 1);
	if(ret < 0){
			return MPU60X0_ERR_CONFIG_ERROR;
	}

	ret = write_command(MPU60X0_defs::GYRO_CONFIG, (uint8_t*)&set->gyro, 1);
	if(ret < 0){
			return MPU60X0_ERR_CONFIG_ERROR;
	}

	return MPU60X0_ERR_OK;
}


uint8_t MPU60X0::who_am_i(){
	uint8_t id = 0xFF;

	read_command(MPU60X0_defs::WHO_AM_I, &id, 1);

	return id & 0x7E;
}

int MPU60X0::check_chip_id(){
	return (who_am_i() ==  chip_id ? MPU60X0_ERR_OK : MPU60X0_ERR_DEV_NOT_FOUND);
}

int MPU60X0::get_temp_raw_data(int16_t* data){
	int ret = read_command(MPU60X0_defs::TEMP_OUT_H, (uint8_t*)data, 2);

	if(ret < 0){
		return MPU60X0_ERR_DATA_ERROR;
	}

	*data = SWAP_16(*data);

	return MPU60X0_ERR_OK;
}

int MPU60X0::get_acc_raw_data(Axis* data){
	int ret = read_command(MPU60X0_defs::ACCEL_XOUT_H, (uint8_t*)data, 6);
	if(ret < 0){
		return MPU60X0_ERR_DATA_ERROR;
	}

	data->x = SWAP_16(data->x);
	data->y = SWAP_16(data->y);
	data->z = SWAP_16(data->z);

	return MPU60X0_ERR_OK;
}

int MPU60X0::get_gyro_raw_data(Axis* data){
	int ret = read_command(MPU60X0_defs::GYRO_XOUT_H, (uint8_t*)data, 6);
	if(ret < 0){
		return MPU60X0_ERR_DATA_ERROR;
	}

	data->x = SWAP_16(data->x);
	data->y = SWAP_16(data->y);
	data->z = SWAP_16(data->z);

	return MPU60X0_ERR_OK;
}

int MPU60X0::get_acc_data(Acc_Scale scale, Axis_Float* data){
	Axis data_i;
	if(get_acc_raw_data(&data_i) != MPU60X0_ERR_OK){
		return MPU60X0_ERR_DATA_ERROR;
	}

	data->x = convert_acc_data(scale, data_i.x);
	data->y = convert_acc_data(scale, data_i.y);
	data->z = convert_acc_data(scale, data_i.z);

	return MPU60X0_ERR_OK;
}


int MPU60X0::get_temp(MPU60X0_FLOAT_TYPE* tempC){
	int16_t temp;
	if(get_temp_raw_data(&temp) < 0){
		return MPU60X0_ERR_DATA_ERROR;
	}

	*tempC = convert_temp_data(temp);

	return MPU60X0_ERR_OK;
}

int MPU60X0::get_gyro_data(Gyro_Scale scale, Axis_Float* data){
	Axis data_i;
	if(get_gyro_raw_data(&data_i) != MPU60X0_ERR_OK){
		return MPU60X0_ERR_DATA_ERROR;
	}

	data->x = convert_gyro_data(scale, data_i.x);
	data->y = convert_gyro_data(scale, data_i.y);
	data->z = convert_gyro_data(scale, data_i.z);

	return MPU60X0_ERR_OK;
}

int MPU60X0::read_fifo_data(uint8_t* data, unsigned int length){
	uint16_t size;
	int ret = read_command(MPU60X0_defs::FIFO_COUNT_H, (uint8_t*)&size, 2);
	if(ret < 0){
		return MPU60X0_ERR_READ_FIFO_SIZE;
	}

	size = SWAP_16(size);
//	return size;
	size = min(length, size);

	for(uint16_t i = 0; i < size; i++){
		ret = read_command(MPU60X0_defs::FIFO_R_W, data, 1);
		if(ret < 0 ){
			return MPU60X0_ERR_READ_FIFO_DATA;
		}
		data++;
	}
	return size;
}

MPU60X0_FLOAT_TYPE MPU60X0::convert_temp_data(int16_t data){
	return ((MPU60X0_FLOAT_TYPE)data/340) + 36.53;
}

MPU60X0_FLOAT_TYPE MPU60X0::convert_acc_data(Acc_Scale scale, int16_t data){
	static const MPU60X0_FLOAT_TYPE RANGE_PER_DIGIT_2G = 0.000061039f;
	MPU60X0_FLOAT_TYPE factor = 0;

	switch(scale){
		case ACC_2G:
			factor = 1;
			break;
		case ACC_4G:
			factor = 2;
			break;
		case ACC_8G:
			factor = 4;
			break;
		case ACC_16G:
			factor = 8;
			break;
	}

	return (MPU60X0_FLOAT_TYPE)data * GRAVITY_EARTH * factor * RANGE_PER_DIGIT_2G;
}

MPU60X0_FLOAT_TYPE MPU60X0::convert_acc_data(settings* set, int16_t data){
	return convert_acc_data(set->acc, data);
}

MPU60X0_FLOAT_TYPE MPU60X0::convert_gyro_data(Gyro_Scale scale, int16_t data){
	static const MPU60X0_FLOAT_TYPE SCALE_DPS_PER_DIGIT_250 = 0.00762986;
	MPU60X0_FLOAT_TYPE factor = 0;

	switch(scale){
		case GYRO_250:
			factor = 1;
			break;
		case GYRO_500:
			factor = 2;
			break;
		case GYRO_1000:
			factor = 4;
			break;
		case GYRO_2000:
			factor = 8;
			break;
	}

	return (MPU60X0_FLOAT_TYPE)data * factor * SCALE_DPS_PER_DIGIT_250;
}

MPU60X0_FLOAT_TYPE MPU60X0::convert_gyro_data(settings* set, int16_t data){
	return convert_gyro_data(set->gyro, data);
}

unsigned int MPU60X0::get_fifo_raw_data(settings* set,
		uint8_t* data, unsigned int length,
		int16_t* temp, Axis* acc /* = NULL */, Axis* gyro /* = NULL */){
	int16_t* data16 = (int16_t*)data;
//	length = length >> 1;
	unsigned int index = 0;


	if((set->fifo_en & acc_en) && acc != NULL){
		if(length > index){
			acc->x = SWAP_16(data16[index]);
			index++;
		}
		if(length > index){
			acc->y = SWAP_16(data16[index]);
			index++;
		}
		if(length > index){
			acc->z = SWAP_16(data16[index]);
			index++;
		}
	}

	if((set->fifo_en & temp_en) && length > index){
		*temp = SWAP_16(data16[index]);
		index++;
	}

	if((set->fifo_en & gyro_en) && gyro != NULL){
		if((set->fifo_en & gyro_x_en) && length > index){
			gyro->x = SWAP_16(data16[index]);
			index++;
		}
		if((set->fifo_en & gyro_y_en) && length > index){
			gyro->y = SWAP_16(data16[index]);
			index++;
		}
		if((set->fifo_en & gyro_z_en) && length > index){
			gyro->z = SWAP_16(data16[index]);
			index++;
		}
	}

	return 2*index;
}

unsigned int MPU60X0::get_fifo_data(settings* set,
		uint8_t* data, unsigned int length,
		MPU60X0_FLOAT_TYPE* temp, Axis_Float* acc /* = NULL */, Axis_Float* gyro /* = NULL */){
	int16_t temp16;
	Axis acc_data, gyro_data;

	unsigned int ret = get_fifo_raw_data(set, data, length,
					&temp16, (acc) ? &acc_data : NULL, (gyro) ? &gyro_data : NULL);

	if((set->fifo_en & acc_en) && acc != NULL){
		acc->x = convert_acc_data(set, acc_data.x);
		acc->y = convert_acc_data(set, acc_data.y);
		acc->z = convert_acc_data(set, acc_data.z);
	}

	if(set->fifo_en & temp_en){
		*temp = convert_temp_data(temp16);
	}

	if((set->fifo_en & gyro_en) && gyro != NULL){
		if(set->fifo_en & gyro_x_en) gyro->x = convert_gyro_data(set, gyro_data.x);
		if(set->fifo_en & gyro_y_en) gyro->y = convert_gyro_data(set, gyro_data.y);
		if(set->fifo_en & gyro_z_en) gyro->z = convert_gyro_data(set, gyro_data.z);
	}

	return ret;
}


/**
 *
 */
int MPU60X0::read_command(uint8_t cmd, uint8_t* aws, unsigned int length){
	return i2c->read(slave_addr, cmd, aws, length, true);
}

int MPU60X0::write_command(uint8_t cmd, uint8_t* data, unsigned int length){
	return i2c->write(slave_addr, cmd, data, length, true);
}
