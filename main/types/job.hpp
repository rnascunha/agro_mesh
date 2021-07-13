#ifndef AGRO_MESH_SCHEDULER_HPP__
#define AGRO_MESH_SCHEDULER_HPP__

#include <cstdint>
#include "rtc_time.hpp"

namespace Jobs{

enum class dow{
	monday = 1 << 0,
	tuesday = 1 << 1,
	wednesday = 1 << 2,
	thursday = 1 << 3,
	friday =  1 << 4,
	saturday = 1 << 5,
	sunday = 1 << 6
};

dow operator|(dow lhs, dow rhs) noexcept;
bool is_dow(value_time) noexcept;

static dow all_dow_days = dow::monday | dow::tuesday | dow::wednesday
				| dow::thursday | dow::friday | dow::saturday
				| dow::sunday;

struct time{
	time();
	time(std::uint8_t, std::uint8_t);
	time(time const&) = default;

	std::uint8_t	hour = 0;
	std::uint8_t	minute = 0;

	bool operator==(time const&) const noexcept;
	bool operator!=(time const&) const noexcept;
	bool operator<(time const&) const noexcept;
	bool operator>(time const&) const noexcept;
};

bool between(value_time, time const& before, time const& after) noexcept;

struct scheduler{
	scheduler(scheduler const&) =  default;
	scheduler(time const& before, time const& after, dow dw_ = all_dow_days);

	time			time_before;
	time			time_after;
	dow				dw = all_dow_days;		///< Day of the week

	bool is_active(value_time) const noexcept;
	operator bool() const noexcept;
};

struct job{
	static constexpr const std::size_t packet_size = 7;

	job(scheduler const& sch_, std::uint8_t prior, std::uint8_t act)
		: sch(sch_), priority(prior), active(act){}

	scheduler		sch;
	std::uint8_t	priority;
	std::uint8_t	active;
};

struct run{

};

}//Jobs

#endif /* AGRO_MESH_SCHEDULER_HPP__ */
