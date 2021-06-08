#ifndef I2C_MASTER_H__
#define I2C_MASTER_H__

#include "driver/i2c.h"
#include "driver/gpio.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define I2C_ERR_OK		1
#define I2C_ERR_WRITE	-1
#define I2C_ERR_READ	-2

#define I2C_FAST_SPEED_HZ	400000
#define I2C_NORMAL_SPPED_HZ	100000

#define I2C_PROBE_ADDR_FUNC

class I2C_Master{
	public:
		I2C_Master(i2c_port_t port, gpio_num_t scl, gpio_num_t sda, uint32_t freq, bool pull_up = false);
		~I2C_Master(){
			deinit();
		}

		void init(int intr_alloc_flags = 0);
		void deinit();

		int write(uint8_t slave_addr, uint8_t reg, uint8_t* data, size_t len, bool ack_check);
		int write(uint8_t slave_addr, uint8_t reg, uint8_t data, bool ack_check);
		int write(uint8_t slave_addr, uint8_t reg, bool ack_check);

		int set_mask(uint8_t slave_addr, uint8_t reg, uint8_t data, uint8_t mask, bool ack_check = true);

		int read(uint8_t slave_addr, uint8_t reg, uint8_t* data, size_t len, bool ack_check);
		int read(uint8_t slave_addr, uint8_t reg, uint8_t* data, bool ack_check = true);

#ifdef I2C_PROBE_ADDR_FUNC
		uint8_t probe_addr(uint8_t addr_init = 0);
#endif /* I2C_PROBE_ADDR_FUNC */
	protected:
		i2c_port_t	port;
};

#endif /* I2C_MASTER_H__ */
