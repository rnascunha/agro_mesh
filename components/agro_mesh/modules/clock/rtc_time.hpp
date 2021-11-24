#ifndef AGRO_DAEMON_RTC_TIME_HPP__
#define AGRO_DAEMON_RTC_TIME_HPP__

#include <cstdint>
#include "ds3231.hpp"

namespace Agro{

using value_time = std::uint32_t;
using fuse_type = int;

class RTC_Time{
	public:
		RTC_Time();

		void init() noexcept;
#if CONFIG_DEVICE_CLOCK_DS3231_SUPPORT
		void init(DS3231& i2c) noexcept;
#endif /* DEVICE_CLOCK_DS3231_SUPPORT */

		bool has_rtc() const noexcept;

		fuse_type fuse() const noexcept;
		void fuse(fuse_type) noexcept;

		value_time get_time() noexcept;
		value_time get_local_time() noexcept;

		value_time internal_time() const noexcept;
		void set_time(value_time time) noexcept;
	private:
#if CONFIG_DEVICE_CLOCK_DS3231_SUPPORT
		DS3231*			rtc_ = nullptr;
		bool			rtc_present_ = false;
#endif /* CONFIG_DEVICE_CLOCK_DS3231_SUPPORT */
		value_time		uptime_sec_ = 0;
		value_time		time_ = 0;
		fuse_type		fuse_;
};

}//Agro

#endif /* AGRO_DAEMON_RTC_TIME_HPP__ */
