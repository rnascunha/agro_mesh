#ifndef BME280_SPI_3W_H__
#define BME280_SPI_3W_H__

#include <stdint.h>
#include "bme280.h"

#include "driver/spi_master.h"

#define BME280_SPI_3W_ERR_CONFIG		-1
#define BME280_SPI_3W_ERR_INIT_BUS		-2
#define BME280_SPI_3W_ERR_INIT_DEV		-3

class BME280_SPI_3W : public BME280{
	public:
		BME280_SPI_3W(spi_device_handle_t device);
		BME280_SPI_3W(spi_host_device_t spi_bus,
				int sclk, int mosi /*sdi*/, int cs);

		int init();
		int soft_reset();

		static int spi_init(spi_host_device_t spi_bus,
				int sclk, int mosi, int cs,
				int freq_hz, spi_device_handle_t* device);

	protected:
		spi_device_handle_t device;

		int read_command(uint8_t cmd, uint8_t* aws, unsigned int length);
		int write_command(uint8_t cmd, uint8_t* data, unsigned int length);
};

#endif /* BME280_SPI_3W_H__ */
