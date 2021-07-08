#include "rtc_time.hpp"
#include "datetime.h"

#include "esp_timer.h"

RTC_Time::RTC_Time(I2C_Master& i2c)
	: rtc_(&i2c)
{}

void RTC_Time::init(I2C_Master& i2c) noexcept
{
	rtc_present_ = i2c.probe(DS3231::reg);
	if(rtc_present_)
		rtc_.begin();
}

bool RTC_Time::has_rtc() const noexcept
{
	return rtc_present_;
}

std::uint32_t RTC_Time::get_time() noexcept
{
	if(rtc_present_)
	{
		DateTime dt;
		rtc_.getDateTime(&dt);
		return dt.getUnixTime();
	}
	return time_ + (static_cast<std::uint32_t>(esp_timer_get_time() / 1000000) - uptime_sec_);
}

std::uint32_t RTC_Time::internal_time() const noexcept
{
	return time_ + (static_cast<std::uint32_t>(esp_timer_get_time() / 1000000) - uptime_sec_);
}

void RTC_Time::set_time(std::uint32_t time) noexcept
{
	if(rtc_present_)
	{
		DateTime dt;
		dt.setUnixTime(time);
		rtc_.setDateTime(&dt);
	}
	time_ = time;
	uptime_sec_ = static_cast<std::uint32_t>(esp_timer_get_time() / 1000000);
}
