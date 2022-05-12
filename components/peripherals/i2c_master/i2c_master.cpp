#include "i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdio.h>

//#define I2C_WRITE_FLAG		0
//#define I2C_READ_FLAG		1

#define TAG "I2C_test"

#ifdef CONFIG_I2C_COMMAND_WAIT
#define I2C_COMMAND_WAIT_MS	CONFIG_I2C_COMMAND_WAIT
#else
#define I2C_COMMAND_WAIT_MS	500
#endif /* CONFIG_I2C_COMMAND_WAIT */

I2C_Master::I2C_Master(i2c_port_t port,
		gpio_num_t scl, gpio_num_t sda,
		uint32_t freq, bool pull_up /* = false */) : port(port){

	i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda;
    conf.sda_pullup_en = pull_up ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    conf.scl_io_num = scl;
    conf.scl_pullup_en = pull_up ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = freq;
    esp_err_t err = i2c_param_config(port, &conf);
    if(err != ESP_OK)
    {
    	printf("I2C config error [%d/%s]\n", err, esp_err_to_name(err));
    }
}

void I2C_Master::init(int intr_alloc_flags /* = 0 */){
	esp_err_t err = i2c_driver_install(port, I2C_MODE_MASTER, 0, 0, intr_alloc_flags);
	if(err != ESP_OK)
	{
		printf("I2C install error [%d/%s]\n", err, esp_err_to_name(err));
	}
}

void I2C_Master::deinit(){
	i2c_driver_delete(port);
}

int I2C_Master::write(uint8_t slave_addr, uint8_t reg, uint8_t* data, size_t len, bool ack_check){
	esp_err_t ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, slave_addr << 1 | I2C_MASTER_WRITE, ack_check);
	i2c_master_write_byte(cmd, reg, ack_check);
	i2c_master_write(cmd, data, len, ack_check);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(port, cmd, I2C_COMMAND_WAIT_MS / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	if (ret != ESP_OK) {
		return I2C_ERR_WRITE;
	}

	return I2C_ERR_OK;
}

int I2C_Master::write(uint8_t slave_addr, uint8_t reg, uint8_t data, bool ack_check /* = true */){
	return write(slave_addr, reg, &data, 1, ack_check);
}

int I2C_Master::write(uint8_t slave_addr, uint8_t reg, bool ack_check /* = true */){
	uint8_t data;
	return write(slave_addr, reg, &data, 0, ack_check);
}


int I2C_Master::set_mask(uint8_t slave_addr, uint8_t reg, uint8_t data, uint8_t mask, bool ack_check /* = true */){
	uint8_t content;

	int ret = read(slave_addr, reg, &content, ack_check);
	if(ret < 0){
		return ret;
	}

	content &= ~mask;
	content |= data & mask;

	return write(slave_addr, reg, content, ack_check);
}


int I2C_Master::read(uint8_t slave_addr, uint8_t reg, uint8_t* data, size_t len, bool ack_check){
	esp_err_t ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, slave_addr << 1 | I2C_MASTER_WRITE, ack_check);
	i2c_master_write_byte(cmd, reg, ack_check);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(port, cmd, I2C_COMMAND_WAIT_MS / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	if (ret != ESP_OK) {
		return I2C_ERR_WRITE;
	}

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, slave_addr << 1 | I2C_MASTER_READ, ack_check);
	if(len > 1){
		i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
	}
	i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(port, cmd, I2C_COMMAND_WAIT_MS / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	if(ret != ESP_OK){
		return I2C_ERR_READ;
	}

	return I2C_ERR_OK;
}

int I2C_Master::read(uint8_t slave_addr, uint8_t reg, uint8_t* data, bool ack_check /* = true */){
	esp_err_t ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, slave_addr << 1 | I2C_MASTER_WRITE, ack_check);
	i2c_master_write_byte(cmd, reg, ack_check);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(port, cmd, I2C_COMMAND_WAIT_MS / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	if (ret != ESP_OK) {
		return I2C_ERR_WRITE;
	}

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, slave_addr << 1 | I2C_MASTER_READ, ack_check);
	i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(port, cmd, I2C_COMMAND_WAIT_MS / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	if(ret != ESP_OK){
		return I2C_ERR_READ;
	}

	return I2C_ERR_OK;
}

bool I2C_Master::probe(uint8_t addr) noexcept
{
	uint8_t data;
	if(read(addr, 0x00, &data) > 0)
		return true;
	return false;
}

uint8_t I2C_Master::probe_addr(uint8_t addr_init /* = 0 */){
	uint8_t data;
	for(uint8_t i = addr_init; i < 127; i++){
		if(read(i, 0x00, &data) > 0){
			return i;
		}
//		delay_ms(10);
	}
	return 0xFF;
}

