#include "bme280_spi.h"
#include "bme280_defs.h"

#include "driver/gpio.h"
#include "esp_err.h"

#define SPI_CLOCK				SPI_MASTER_FREQ_10M

BME280_SPI::BME280_SPI(spi_device_handle_t device) :
	device(device){}

BME280_SPI::BME280_SPI(spi_host_device_t spi_bus,
		int sclk, int mosi, int miso, int cs){
	BME280_SPI::spi_init(spi_bus,
				sclk, mosi, miso, cs,
				SPI_CLOCK, &device);
}


int BME280_SPI::init(){
	int ret = check_chip_id();

	if(ret == BME280_ERR_CHIP_ID){
		return BME280_ERR_CHIP_ID;
	}

	ret = soft_reset();
	if(ret < 0){
		return ret;
	}

	ret = get_calib_data();
	if(ret < 0){
		return ret;
	}

	return BME280_ERR_OK;
}

int BME280_SPI::spi_init(spi_host_device_t spi_bus,
		int sclk, int mosi, int miso, int cs,
		int freq_hz, spi_device_handle_t* device){
	esp_err_t ret;

	spi_bus_config_t bus_cfg = {};
	bus_cfg.miso_io_num = miso;
	bus_cfg.mosi_io_num = mosi;
	bus_cfg.sclk_io_num = sclk;
	bus_cfg.quadwp_io_num = GPIO_NUM_NC;
	bus_cfg.quadhd_io_num = GPIO_NUM_NC;
	bus_cfg.max_transfer_sz = 26;

	ret = spi_bus_initialize(spi_bus, &bus_cfg, 0);		//No DMA
	if(ret != ESP_OK){
		return BME280_SPI_ERR_INIT_BUS;
	}

	spi_device_interface_config_t dev_cfg = {};
	dev_cfg.clock_speed_hz = freq_hz;	           	//Clock out at 10 MHz
	dev_cfg.mode = 0;                   	       	//SPI mode 0
	dev_cfg.spics_io_num = cs;             	   		//CS pin
	dev_cfg.queue_size = 1;                        	//We want to be able to queue 1 transactions at a time
	dev_cfg.command_bits = 8;

	ret = spi_bus_add_device(spi_bus, &dev_cfg, device);
	if(ret != ESP_OK){
		return BME280_SPI_ERR_INIT_DEV;
	}

	return BME280_ERR_OK;
}

/**
 *
 */
int BME280_SPI::read_command(uint8_t cmd, uint8_t* aws, unsigned int length){
	esp_err_t ret;

	spi_transaction_t t = {};

	t.cmd = cmd | 0x80;
	t.length = length * 8;						//Command is 8 bits
	t.rx_buffer = aws;

	ret = spi_device_polling_transmit(device, &t);  //Transmit!

	if(ret != ESP_OK){
		return -1;
	}

	return 1;
}


int BME280_SPI::write_command(uint8_t cmd, uint8_t* data, unsigned int length){
	esp_err_t ret;

	spi_transaction_t t = {};

	t.cmd = cmd & 0x7F;
	t.length = length * 8;						//Command is 8 bits
	t.tx_buffer = data;

	ret = spi_device_polling_transmit(device, &t);  //Transmit!

	if(ret != ESP_OK){
		return -1;
	}

	return 1;
}

