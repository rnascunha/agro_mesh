#ifndef BME280_I2C_H__
#define BME280_I2C_H__

#include "i2c_master.h"
#include "bme280.h"

/**
 * Pinos:
 * CSB: VDDIO para selecionar I2C;
 * SDO: Bit 0 do endereço I2C
 */

class BME280_I2C : public BME280{
	public:
		BME280_I2C(I2C_Master* i2c);

		int init();
		int init(bool sdo);
	protected:
		I2C_Master *i2c;
		uint8_t slave_addr;

		virtual int read_command(uint8_t cmd, uint8_t* aws, unsigned int length);
		virtual int write_command(uint8_t cmd, uint8_t* data, unsigned int length);
};

#endif /* BME280_I2C_H__ */
