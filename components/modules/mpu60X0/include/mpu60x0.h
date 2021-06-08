#ifndef MPU60X0_H__
#define MPU60X0_H__

#include "i2c_master.h"
#include <stdint.h>

/**
 * TODO Usar as interrupcoes
 *
 * TODO Verificar cycle mode. Nao esta funcionando corretamente
 */

/**
 * Pinos:
 * AD0: bit 0 (LSB) do endereço I2C
 */

#define MPU60X0_ERR_OK				1
#define MPU60X0_ERR_DEV_NOT_FOUND	-1
#define MPU60X0_ERR_DATA_ERROR		-2
#define MPU60X0_ERR_CONFIG_ERROR	-3
#define MPU60X0_ERR_READ_FIFO_SIZE	-4
#define MPU60X0_ERR_READ_FIFO_DATA	-5

#define MPU60X0_FLOAT_TYPE			double
#define MPU60X0_BUFFER_SIZE			1024

class MPU60X0 {
	public:
		static const uint8_t chip_id = 0x68;

		enum Acc_Scale{
			ACC_2G				= 0b00000000,
			ACC_4G				= 0b00001000,
			ACC_8G				= 0b00010000,
			ACC_16G				= 0b00011000
		};

		enum Gyro_Scale{
			GYRO_250			= 0b00000000,
			GYRO_500			= 0b00001000,
			GYRO_1000			= 0b00010000,
			GYRO_2000			= 0b00011000
		};

		enum Power_Mgmt_1{
			Internal_Osc_8Mhz	= 0b000,
			GYRO_X_REF			= 0b001,
			GYRO_Y_REF			= 0b010,
			GYRO_Z_REF			= 0b011,
			External_32KHz		= 0b100,
			Clock_Stop			= 0b111,
			Temp_Disable		= 0b00001000,
			Cycle				= 0b00100000,
			Sleep				= 0b01000000,
			Reset_Device		= 0b10000000,
		};

		enum Power_Mgmt_2{
			LP_Wake_1_25Hz		= 0b00000000,
			LP_Wake_5Hz			= 0b01000000,
			LP_Wake_20Hz		= 0b10000000,
			LP_Wake_40Hz		= 0b11000000,
			Stand_By_Acc_X		= 0b00100000,
			Stand_By_Acc_Y		= 0b00010000,
			Stand_By_Acc_Z		= 0b00001000,
			Stand_By_Gyro_X		= 0b00000100,
			Stand_By_Gyro_Y		= 0b00000010,
			Stand_By_Gyro_Z		= 0b00000001
		};

		enum Config{
			Low_Pass_260Hz		= 0b000,
			Low_Pass_184Hz		= 0b001,
			Low_Pass_94Hz		= 0b010,
			Low_Pass_44Hz		= 0b011,
			Low_Pass_21Hz		= 0b100,
			Low_Pass_10Hz		= 0b101,
			Low_Pass_5Hz		= 0b110,
		};

		enum FIFO_Enable{
			acc_en 				= 0b00001000,
			gyro_z_en 			= 0b00010000,
			gyro_y_en 			= 0b00100000,
			gyro_x_en 			= 0b01000000,
			gyro_en				= gyro_x_en | gyro_y_en | gyro_z_en,
			temp_en				= 0b10000000
		};

		struct Axis{
			int16_t x;
			int16_t y;
			int16_t z;
		};

		struct Axis_Float{
			MPU60X0_FLOAT_TYPE x;
			MPU60X0_FLOAT_TYPE y;
			MPU60X0_FLOAT_TYPE z;
		};

		struct settings{
			settings() : pwr_mgmt_1(0),	pwr_mgmt_2(0),
					config((uint8_t)Low_Pass_260Hz),
					sample_rate_div(0), fifo_en(0),
					acc(ACC_2G), gyro(GYRO_250){}

			uint8_t pwr_mgmt_1;
			uint8_t pwr_mgmt_2;
			uint8_t config;
			uint8_t sample_rate_div;
			uint8_t fifo_en;
			Acc_Scale acc;
			Gyro_Scale gyro;
		};

		MPU60X0(I2C_Master *i2c);

		int init();
		int init(bool ad0);

		int config(settings* set);

		int get_temp_raw_data(int16_t* data);
		int get_acc_raw_data(Axis* data);
		int get_gyro_raw_data(Axis* data);

		int get_temp(MPU60X0_FLOAT_TYPE* temp);
		int get_acc_data(Acc_Scale scale, Axis_Float* data);
		int get_gyro_data(Gyro_Scale scale, Axis_Float* data);

		int read_fifo_data(uint8_t* data, unsigned int length);

		uint8_t who_am_i();
		int check_chip_id();

		static unsigned int get_fifo_raw_data(settings* set,
				uint8_t* data, unsigned int length,
				int16_t* temp, Axis* acc = NULL, Axis* gyro = NULL);

		static unsigned int get_fifo_data(settings* set,
				uint8_t* data, unsigned int length,
				MPU60X0_FLOAT_TYPE* temp, Axis_Float* acc = NULL, Axis_Float* gyro = NULL);

		static MPU60X0_FLOAT_TYPE convert_temp_data(int16_t data);

		static MPU60X0_FLOAT_TYPE convert_acc_data(Acc_Scale scale, int16_t data);
		static MPU60X0_FLOAT_TYPE convert_acc_data(settings* scale, int16_t data);

		static MPU60X0_FLOAT_TYPE convert_gyro_data(Gyro_Scale scale, int16_t data);
		static MPU60X0_FLOAT_TYPE convert_gyro_data(settings* scale, int16_t data);
	protected:
		I2C_Master *i2c;
		uint8_t slave_addr;

		int write_command(uint8_t reg, uint8_t* data, unsigned int len);
		int read_command(uint8_t reg, uint8_t* data, unsigned int len);
};

#endif /* MPU60X0_H__ */
