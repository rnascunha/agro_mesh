#include "job.hpp"

#include "gpio.h"
#include "datetime.h"
#include "../storage.hpp"

extern RTC_Time device_clock;
extern GPIO_Basic ac_load[];
extern const std::size_t ac_load_count;

static constexpr const std::uint8_t default_act = 0;

namespace Jobs{

dow operator|(dow lhs, dow rhs) noexcept
{
	return static_cast<dow>(static_cast<int>(rhs) | static_cast<int>(lhs));
}

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

scheduler::scheduler(time const& before, time const& after, dow dw_ /* = all_dow_days */)
	: time_before(before), time_after(after), dw(dw_){}

bool scheduler::is_active(value_time t) const noexcept
{
	return between(t, time_before, time_after) && is_dow(dw, t);
}

bool scheduler::operator()() const noexcept
{
	return is_active(device_clock.get_local_time());
}

bool scheduler::operator==(scheduler const& rhs) const noexcept
{
	return dw == rhs.dw &&
			time_before == rhs.time_before &&
			time_after == rhs.time_after;
}

void execute(std::uint8_t act) noexcept
{
	for(std::size_t i = 0; i < ac_load_count; i++)
	{
		ac_load[i].write(act & (1 << i) ? 1 : 0);
	}
}

void job::execute() const noexcept
{
	Jobs::execute(active);
}

bool job::operator==(job const& rhs) noexcept
{
	return rhs.active == active
			&& rhs.priority == priority
			&& rhs.sch == sch;
}

run::run(const char* path) :
	path_(path){}

void run::clear() noexcept
{
	job_.priority = 0;
	job_.active = default_act;
}

bool run::is_clear() const noexcept
{
	return job_.priority == 0;
}

bool run::check(int& index, bool force /* = false */) noexcept
{
	index = 0;
	bool ret = false;
	FILE *f = fopen(path_, "rb");
	if (f == NULL)
	{
		if(!is_clear())
		{
			clear();
			ret = true;
		}
		job_.execute();
		return ret;
	}

	if(!is_clear() && !job_.sch())
	{
		clear();
		ret = true;
	}

	if(force)
	{
		clear();
	}

	std::uint8_t data[job::packet_size];
	std::size_t readed = 0;
	int count = 1;
	do{
		readed = fread(data, 1, job::packet_size, f);
		if(!readed) break;
		Jobs::job jr{
					Jobs::scheduler{
						Jobs::time{data[0], data[1]},
						Jobs::time{data[2], data[3]},
						static_cast<Jobs::dow>(data[4])},
					data[5], data[6]};
		if(jr.sch() && jr.priority > job_.priority)
		{
			index = count;
			job_ = jr;
			ret = true;
		}
		count++;
	}while(true);
	fclose(f);

//	if(ret)
	job_.execute();

	return ret;
}

bool run::check(bool force /* = false */) noexcept
{
	int index;
	return check(index, force);
}

}//Jobs
