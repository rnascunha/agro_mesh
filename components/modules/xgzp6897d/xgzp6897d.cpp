#include "xgzp6897d.hpp"
//#include <stdio.h>

enum class reg : uint8_t{
	data_msb = 0x06,
	data_csb = 0x07,
	data_lsb = 0x08,
	temp_msb = 0x09,
	temp_lsb = 0x0a,
	cmd = 0x30,
	sys_config = 0xa5,
	p_config = 0xa6,
	t_config_1 = 0xa7
};

static constexpr const uint8_t start_conversion_value = 0b1000;

XGZP6897D::XGZP6897D(I2C_Master& i2c)
	: i2c_(&i2c){}

int XGZP6897D::init() noexcept
{
//	uint8_t data;
//	int ret = i2c_->read(address, static_cast<uint8_t>(reg::sys_config), &data, true);
//	if(ret != I2C_ERR_OK)
//	{
//		return ret;
//	}
//
//	data &= 0b11111101;
//	return i2c_->write(address, static_cast<uint8_t>(reg::sys_config), data, true);
	return set_value(static_cast<uint8_t>(reg::sys_config), 0, 0b11111101);
}

int XGZP6897D::start_conversion(measurement_control control) noexcept
{
	return i2c_->write(address,
			static_cast<uint8_t>(reg::cmd),
			static_cast<uint8_t>(control) | start_conversion_value,
			true);
}

int XGZP6897D::check_conversion() noexcept
{
	uint8_t data;
	i2c_->read(address, static_cast<uint8_t>(reg::cmd), &data);
	return !(data & start_conversion_value);
}

void XGZP6897D::wait_conversion() noexcept
{
	while(!check_conversion()){}
}

int XGZP6897D::set_gain(gain g) noexcept
{
	return set_value(static_cast<uint8_t>(reg::p_config),
				static_cast<uint8_t>(g),
				0b11000111);
}

int XGZP6897D::set_sampling(sampling s) noexcept
{
	return set_value(static_cast<uint8_t>(reg::p_config),
			static_cast<uint8_t>(s),
			0b11111000);
}

int XGZP6897D::read_adc_pressure(uint32_t& pressure) noexcept
{
//	uint8_t data[3] = {0,};
//	int ret = i2c_->read(address, static_cast<uint8_t>(reg::data_msb), data, 3, true);
//
//	if(ret != I2C_ERR_OK)
//	{
//		return ret;
//	}
//
//	printf("Reg: %u %u %u\n", data[0], data[1], data[2]);
//
//	pressure = (data[0] << 16) | (data[1] << 8) | data[2];
//
//	return ret;

	uint8_t a = 0, b = 0, c = 0;
	i2c_->read(address, static_cast<uint8_t>(reg::data_msb), &a, true);
	i2c_->read(address, static_cast<uint8_t>(reg::data_csb), &b, true);
	i2c_->read(address, static_cast<uint8_t>(reg::data_lsb), &c, true);

	return 1;
}

int XGZP6897D::read_adc_temperature(uint16_t& temp) noexcept
{
//	uint8_t data[2] = {0,};
//	int ret = i2c_->read(address, static_cast<uint8_t>(reg::temp_msb), data, 2, true);
//
//	if(ret != I2C_ERR_OK)
//	{
//		return ret;
//	}
//
//	printf("Reg: %u %u\n", data[0], data[1]);
//
//	temp = (data[0] << 8) | data[1];
//
//	return ret;
	uint8_t a = 0, b = 0;
	i2c_->read(address, static_cast<uint8_t>(reg::temp_msb), &a, true);
	i2c_->read(address, static_cast<uint8_t>(reg::temp_lsb), &b, true);

	temp = (a << 8) | b;

	return 1;
}

int XGZP6897D::read_adc(uint16_t& temp, uint32_t& pressure) noexcept
{
	uint8_t data[5] = {0,};
	int ret = i2c_->read(address, static_cast<uint8_t>(reg::data_msb), data, 5, true);

	if(ret != I2C_ERR_OK)
	{
		return ret;
	}

	pressure = (data[0] << 16) | (data[1] << 8) | data[2];
	temp = (data[3] << 8) | data[4];

	return ret;
//	uint8_t a = 0, b = 0, c = 0, d = 0, e = 0;
//	i2c_->read(address, static_cast<uint8_t>(reg::data_msb), &a, true);
//	i2c_->read(address, static_cast<uint8_t>(reg::data_csb), &b, true);
//	i2c_->read(address, static_cast<uint8_t>(reg::data_lsb), &c, true);
//
//	i2c_->read(address, static_cast<uint8_t>(reg::temp_msb), &d, true);
//	i2c_->read(address, static_cast<uint8_t>(reg::temp_lsb), &e, true);
//
//	printf("Reg: %u %u %u %u %u\n", a, b, c, d, e);
//
//	pressure = (a << 16) | (b << 8) | c;
//	temp = (d << 8) | e;
//
//	return I2C_ERR_OK;
}

int XGZP6897D::read_temperature(double& temp) noexcept
{
	uint16_t temp_adc;
	int ret = read_adc_temperature(temp_adc);
	if(ret != I2C_ERR_OK)
	{
		return ret;
	}

	temp = convert_temperature(temp_adc);
	return ret;
}

int XGZP6897D::read_pressure(double& pressure, unsigned k) noexcept
{
	uint32_t pressure_adc;
	int ret = read_adc_pressure(pressure_adc);
	if(ret != I2C_ERR_OK)
	{
		return ret;
	}

	pressure = convert_pressure(pressure_adc, k);

	return ret;
}

int XGZP6897D::read_data(double& temp, double& pressure, unsigned k) noexcept
{
	uint16_t temp_adc;
	uint32_t pressure_adc;
	int ret = read_adc(temp_adc, pressure_adc);

	if(ret != I2C_ERR_OK)
	{
		return ret;
	}

	temp = convert_temperature(temp_adc);
	pressure = convert_pressure(pressure_adc, k);

	return ret;
}

double XGZP6897D::convert_temperature(uint16_t temp_adc) noexcept
{
	return (static_cast<double>(temp_adc) - (temp_adc & 0x8000 ? 65536.0 : 0.0)) / 256.0;
}

double XGZP6897D::convert_pressure(uint32_t pressure_adc, unsigned k) noexcept
{
	return (static_cast<double>(pressure_adc) - (pressure_adc & 0x800000 ? 16777216.0 : 0.0)) / k;
}

/**
 *
 */
int XGZP6897D::set_value(uint8_t reg, uint8_t data, uint8_t mask) noexcept
{
	uint8_t data_r;
	int ret = i2c_->read(address, reg, &data_r, true);
	if(ret != I2C_ERR_OK)
	{
		return ret;
	}

	data_r = (mask & data_r) | data;

	return i2c_->write(address, reg, data_r, true);
}
