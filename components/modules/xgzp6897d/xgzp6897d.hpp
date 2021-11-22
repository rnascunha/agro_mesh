#ifndef MODULE_XGZP6897_HPP__
#define MODULE_XGZP6897_HPP__

#include "i2c_master.h"
#include <stdint.h>

/**
 * The K value to convert the ADC pressure data read to Pa must be checked at
 * the datasheet. It depends of which pressure range sensor you are using.
 *
 * product page: http://www.cfsensor.com/proinfo/28.html
 */

class XGZP6897D{
	public:
		static constexpr const uint8_t address = 0x6d;

		XGZP6897D(I2C_Master& i2c);

		enum class measurement_control : uint8_t{
			temperature_x1 = 0b000,
			pressure_x1 = 0b001,
			temp_pressure_x1 = 0b010,
			sleep_conversion = 0b011,
			otp_programing_mode = 0b100
		};

		enum class gain : uint8_t{
			x1 = 0b000000,
			x2 = 0b001000,
			x4 = 0b010000,
			x8 = 0b011000,
			x16 = 0b100000,
			x32 = 0b101000,
			x64 = 0b110000,
			x128 = 0b111000
		};

		enum class sampling : uint8_t{
			x1024 = 0b000,
			x2048 = 0b001,
			x4096 = 0b010,
			x8192 = 0b011,
			x256 = 0b100,
			x512 = 0b101,
			x16384 = 0b110,
			x32768 = 0b111
		};

		int init() noexcept;

		int start_conversion(measurement_control) noexcept;
		int check_conversion() noexcept;
		void wait_conversion() noexcept;

		int set_gain(gain) noexcept;
		int set_sampling(sampling) noexcept;

		int set_temperature_gain(gain) noexcept;
		int set_temperature_sampling(sampling) noexcept;

		int read_adc_pressure(uint32_t&) noexcept;
		int read_adc_temperature(uint16_t&) noexcept;
		int read_adc(uint16_t& temp, uint32_t& pressure) noexcept;

		int read_temperature(double&) noexcept;
		int read_pressure(double&, unsigned k) noexcept;
		int read_data(double& temp, double& pressure, unsigned k) noexcept;

		static double convert_temperature(uint16_t temp_adc) noexcept;
		static double convert_pressure(uint32_t pressure_adc, unsigned k) noexcept;
	private:
		int set_value(uint8_t reg, uint8_t data, uint8_t mask) noexcept;

		I2C_Master* i2c_;
};

#endif /* MODULE_XGZP6897_HPP__ */
