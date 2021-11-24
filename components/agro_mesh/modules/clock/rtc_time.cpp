#include "rtc_time.hpp"
#include "datetime.h"

#include "nvs_flash.h"
#include "esp_timer.h"

namespace Agro{

static constexpr const fuse_type default_fuse = -3 * 60 * 60;

RTC_Time::RTC_Time()
	: fuse_(default_fuse)
{}

void RTC_Time::init() noexcept
{
	nvs_handle_t handle;
	esp_err_t err = nvs_open("storage", NVS_READONLY, &handle);
	if (err == ESP_OK)
	{
		int32_t nfuse;
		err = nvs_get_i32(handle, "fuse", &nfuse);
		switch (err)
		{
			case ESP_OK:
				fuse_ = nfuse;
				break;
			case ESP_ERR_NVS_NOT_FOUND:
				break;
			default :
				break;
		}
		// Close
		nvs_close(handle);
	}
}

#if CONFIG_DEVICE_CLOCK_DS3231_SUPPORT
void RTC_Time::init(DS3231& rtc) noexcept
{
	rtc_present_ = rtc.probe();
	if(rtc_present_)
	{
		rtc_ = &rtc;
		rtc_->begin();
	}

	init();
}
#endif /* CONFIG_DEVICE_CLOCK_DS3231_SUPPORT */

bool RTC_Time::has_rtc() const noexcept
{
#if CONFIG_DEVICE_CLOCK_DS3231_SUPPORT
	return rtc_present_;
#else
	return false;
#endif /* CONFIG_DEVICE_CLOCK_DS3231_SUPPORT */
}

fuse_type RTC_Time::fuse() const noexcept
{
	return fuse_;
}

void RTC_Time::fuse(fuse_type f) noexcept
{
	nvs_handle_t handle;
	esp_err_t err = nvs_open("storage", NVS_READWRITE, &handle);
	if (err == ESP_OK)
	{
		err = nvs_set_i32(handle, "fuse", f);
		err = nvs_commit(handle);
		// Close
		nvs_close(handle);
	}
	fuse_ = f;
}

value_time RTC_Time::get_time() noexcept
{
#if CONFIG_DEVICE_CLOCK_DS3231_SUPPORT
	if(rtc_present_)
	{
		DateTime dt;
		rtc_->getDateTime(&dt);
		return dt.getUnixTime();
	}
#endif /* CONFIG_DEVICE_CLOCK_DS3231_SUPPORT */
	return internal_time();
}

value_time RTC_Time::get_local_time() noexcept
{
	return get_time() + fuse_;
}

value_time RTC_Time::internal_time() const noexcept
{
	return time_ + (static_cast<value_time>(esp_timer_get_time() / 1000000) - uptime_sec_);
}

void RTC_Time::set_time(std::uint32_t time) noexcept
{
#if CONFIG_DEVICE_CLOCK_DS3231_SUPPORT
	if(rtc_present_)
	{
		DateTime dt;
		dt.setUnixTime(time);
		rtc_->setDateTime(&dt);
	}
#endif /* CONFIG_DEVICE_CLOCK_DS3231_SUPPORT */
	time_ = time;
	uptime_sec_ = static_cast<value_time>(esp_timer_get_time() / 1000000);
}

}//Agro
