#ifndef AGRO_DAEMON_RTC_TIME_HPP__
#define AGRO_DAEMON_RTC_TIME_HPP__

#include <cstdint>
#include "i2c_master.h"
#include "ds3231.hpp"

using value_time = std::uint32_t;
using fuse_type = int;

class RTC_Time{
	public:
		RTC_Time(I2C_Master& i2c);

		void init(I2C_Master& i2c) noexcept;

		bool has_rtc() const noexcept;

		fuse_type fuse() const noexcept;
		void fuse(fuse_type) noexcept;

		value_time get_time() noexcept;
		value_time get_local_time() noexcept;

		value_time internal_time() const noexcept;
		void set_time(value_time time) noexcept;
	private:
		DS3231			rtc_;
		bool			rtc_present_ = false;
		value_time		uptime_sec_ = 0;
		value_time		time_ = 0;
		fuse_type		fuse_;
};

#endif /* AGRO_DAEMON_RTC_TIME_HPP__ */
