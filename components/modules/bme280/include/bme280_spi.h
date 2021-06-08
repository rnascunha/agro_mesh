#ifndef BME280_SPI_H__
#define BME280_SPI_H__

#include <stdint.h>
#include "bme280.h"

#include "driver/spi_master.h"

#define BME280_SPI_ERR_INIT_BUS		-1
#define BME280_SPI_ERR_INIT_DEV		-2

class BME280_SPI : public BME280{
	public:
		BME280_SPI(spi_device_handle_t device);
		BME280_SPI(spi_host_device_t spi_bus,
				int sclk, int mosi, int miso, int cs);

		int init();
		static int spi_init(spi_host_device_t spi_bus,
				int sclk, int mosi, int miso, int cs,
				int freq_hz, spi_device_handle_t* device);

	protected:
		spi_device_handle_t device;

		int read_command(uint8_t cmd, uint8_t* aws, unsigned int length);
		int write_command(uint8_t cmd, uint8_t* data, unsigned int length);
};

#endif /* BME280_SPI_H__ */
