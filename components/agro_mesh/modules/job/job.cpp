#include "job.hpp"

#include "datetime.h"
#include "../storage.hpp"
#include "../clock/rtc_time.hpp"

#include "../clock/helper.hpp"

namespace Agro{
namespace Jobs{

bool is_dow(dow d, value_time t) noexcept
{
	DateTime dt;
	dt.setUnixTime(t);
	return static_cast<int>(d) & (1 << (static_cast<int>(dt.dayOfWeek()) - 1));
}

time::time(){}
time::time(std::uint8_t h, std::uint8_t m)
	: hour(h > 23 ? 23 : h), minute(m > 59 ? 59 : m){}

bool time::operator==(time const& rhs) const noexcept
{
	return hour == rhs.hour && minute == rhs.minute;
}

bool time::operator!=(time const& rhs) const noexcept
{
	return !(*this == rhs);
}

bool time::operator<(time const& rhs) const noexcept
{
	if(hour > rhs.hour) return false;
	if(hour < rhs.hour) return true;
	if(minute > rhs.minute) return false;
	if(minute < rhs.minute) return true;
	return false;
}

bool time::operator>(time const& rhs) const noexcept
{
	return *this != rhs && !(*this < rhs);
}

bool time::operator>=(time const& rhs) const noexcept
{
	return !(*this < rhs);
}

bool time::operator<=(time const& rhs) const noexcept
{
	return !(*this > rhs);
}

bool between(value_time t, time const& before, time const& after) noexcept
{
	DateTime dt;
	dt.setUnixTime(t);
	time t_{dt.getHour(), dt.getMinute()};

	if(before < after)
		return t_ >= before && t_ < after;
	else if(after < before)
		return t_ >= before || t_ < after;
	return t_ == after;
}

schedule::schedule(time const& before, time const& after, dow dw_ /* = all_dow_days */)
	: time_before(before), time_after(after), dw(dw_){}

bool schedule::is_active(value_time t) const noexcept
{
	return between(t, time_before, time_after) && is_dow(dw, t);
}

bool schedule::operator()() const noexcept
{
	return is_active(get_local_time());
}

bool schedule::operator==(schedule const& rhs) const noexcept
{
	return dw == rhs.dw &&
			time_before == rhs.time_before &&
			time_after == rhs.time_after;
}

}//Jobs
}//Agro
