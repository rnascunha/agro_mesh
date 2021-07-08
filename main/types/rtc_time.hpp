#ifndef AGRO_DAEMON_RTC_TIME_HPP__
#define AGRO_DAEMON_RTC_TIME_HPP__

#include <cstdint>
#include "i2c_master.h"
#include "ds3231.hpp"

class RTC_Time{
	public:
		RTC_Time(I2C_Master& i2c);

		void init(I2C_Master& i2c) noexcept;

		bool has_rtc() const noexcept;

		std::uint32_t get_time() noexcept;
		std::uint32_t internal_time() const noexcept;
		void set_time(uint32_t time) noexcept;
	private:
		DS3231			rtc_;
		bool			rtc_present_ = false;
		std::uint32_t	uptime_sec_ = 0;
		std::uint32_t	time_ = 0;
};

#endif /* AGRO_DAEMON_RTC_TIME_HPP__ */
