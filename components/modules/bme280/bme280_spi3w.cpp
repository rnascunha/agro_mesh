#include "bme280_spi3w.h"
#include "bme280_defs.h"

#include "driver/gpio.h"
#include "esp_err.h"

#include "hardware_defs.h"

#define SPI_CLOCK				SPI_MASTER_FREQ_10M

BME280_SPI_3W::BME280_SPI_3W(spi_device_handle_t device) :
	device(device){}

BME280_SPI_3W::BME280_SPI_3W(spi_host_device_t spi_bus,
		int sclk, int mosi, int cs){
	BME280_SPI_3W::spi_init(spi_bus,
				sclk, mosi, cs,
				SPI_CLOCK, &device);
}

int BME280_SPI_3W::init(){
	int ret;

	uint8_t cmd = 0x01;
	ret = write_command(BME280_defs::CONFIG, &cmd, 1);
	if(ret < 0){
		return BME280_SPI_3W_ERR_CONFIG;
	}

	ret = check_chip_id();

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

int BME280_SPI_3W::soft_reset(){
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
    	/**
    	 * Reeconfigurando o dispositivo como SPI 3W
    	 */
    	uint8_t cmd_spi = 0x01;
    	write_command(BME280_defs::CONFIG, &cmd_spi, 1);
    }while ((ret > 0) && (try_run--) && (status_reg & BME280_defs::IM_UPDATE));

    if (status_reg & BME280_defs::IM_UPDATE){
        return BME280_ERR_SOFT_RESET_WAIT;
    }

	return BME280_ERR_OK;
}

int BME280_SPI_3W::spi_init(spi_host_device_t spi_bus,
		int sclk, int mosi /* sdi */, int cs,
		int freq_hz, spi_device_handle_t* device){
	esp_err_t ret;

	spi_bus_config_t bus_cfg = {};
	bus_cfg.miso_io_num = GPIO_NUM_NC;
	bus_cfg.mosi_io_num = mosi;
	bus_cfg.sclk_io_num = sclk;
	bus_cfg.quadwp_io_num = GPIO_NUM_NC;
	bus_cfg.quadhd_io_num = GPIO_NUM_NC;
	bus_cfg.max_transfer_sz = 26;

	ret = spi_bus_initialize(spi_bus, &bus_cfg, 0);		//No DMA
	if(ret != ESP_OK){
		return BME280_SPI_3W_ERR_INIT_BUS;
	}

	spi_device_interface_config_t dev_cfg = {};
	dev_cfg.clock_speed_hz = freq_hz;	           	//Clock out at 10 MHz
	dev_cfg.mode = 0;                   	       	//SPI mode 0
	dev_cfg.spics_io_num = cs;             	   		//CS pin
	dev_cfg.queue_size = 1;                        	//We want to be able to queue 1 transactions at a time
	dev_cfg.command_bits = 8;
	dev_cfg.flags = SPI_DEVICE_3WIRE | SPI_DEVICE_HALFDUPLEX;

	ret = spi_bus_add_device(spi_bus, &dev_cfg, device);
	if(ret != ESP_OK){
		return BME280_SPI_3W_ERR_INIT_DEV;
	}

	return BME280_ERR_OK;
}


/**
 *
 */
int BME280_SPI_3W::read_command(uint8_t cmd, uint8_t* aws, unsigned int length){
	esp_err_t ret;

	spi_transaction_t t = {};

	t.cmd = cmd | 0x80;
	t.rxlength = length * 8;
	t.rx_buffer = aws;

	ret = spi_device_polling_transmit(device, &t);  //Transmit!

	if(ret != ESP_OK){
		return -1;
	}

	return 1;
}

int BME280_SPI_3W::write_command(uint8_t cmd, uint8_t* data, unsigned int length){
	esp_err_t ret;

	spi_transaction_t t = {};

	t.cmd = cmd & 0x7F;
	t.length = (length) * 8;						//Command is 8 bits
	t.tx_buffer = data;

	ret = spi_device_polling_transmit(device, &t);  //Transmit!

	if(ret != ESP_OK){
		return -1;
	}

	return 1;
}

