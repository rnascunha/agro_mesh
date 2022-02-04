#include "helper.hpp"

extern Agro::RTC_Time device_clock;

namespace Agro{

value_time get_time() noexcept
{
	return device_clock.get_time();
}

value_time get_local_time() noexcept
{
	return device_clock.get_local_time();
}

DateTime get_datetime() noexcept
{
	DateTime dt;
	dt.setUnixTime(device_clock.get_time());

	return dt;
}

DateTime get_local_datetime() noexcept
{
	DateTime dt;
	dt.setUnixTime(device_clock.get_local_time());

	return dt;
}

}//Agro
