#ifndef AGRO_MESH_SCHEDULER_HPP__
#define AGRO_MESH_SCHEDULER_HPP__

#include <cstdint>
#include "../clock/rtc_time.hpp"

namespace Agro{
namespace Jobs{

enum class dow : std::uint8_t{
	monday = 1 << 0,
	tuesday = 1 << 1,
	wednesday = 1 << 2,
	thursday = 1 << 3,
	friday =  1 << 4,
	saturday = 1 << 5,
	sunday = 1 << 6
};

inline constexpr dow operator|(dow lhs, dow rhs) noexcept
{
	return static_cast<dow>(static_cast<int>(rhs) | static_cast<int>(lhs));
}

bool is_dow(value_time) noexcept;

static constexpr const dow all_dow_days = dow::monday | dow::tuesday | dow::wednesday
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
	bool operator>=(time const&) const noexcept;
	bool operator<=(time const&) const noexcept;

	time& operator=(time const&) noexcept = default;
};

bool between(value_time, time const& before, time const& after) noexcept;

struct schedule{
	schedule() = default;
	schedule(schedule const&) =  default;
	schedule(time const& before, time const& after, dow dw_ = all_dow_days);

	time			time_before;
	time			time_after;
	dow				dw = all_dow_days;		///< Day of the week

	bool operator==(schedule const&) const noexcept;
	schedule& operator=(schedule const&) noexcept = default;
	bool is_active(value_time) const noexcept;
	bool operator()() const noexcept;
};

struct __attribute__((packed)) job_packet{
	std::uint8_t be_time_hour;
	std::uint8_t be_time_minute;
	std::uint8_t af_time_hour;
	std::uint8_t af_time_minute;
	dow			 day_of_week;
	std::uint8_t priority;
};

template<typename Executor>
struct job{
	static constexpr const std::size_t packet_size = sizeof(job_packet) + Executor::packet_size;

	job() = default;

	schedule		sch;
	std::uint8_t	priority = 0;
	Executor		exec;

	bool read(FILE*) noexcept;

	void execute() noexcept;
	void start() noexcept;
	void stop() noexcept;

	job& operator=(job&&) noexcept;
};

enum class run_error{
	success = 0,
	file_open,
};

template<typename Executor,
		typename Config>
class run{
	public:
		static constexpr const int no_exec = -1;
		static constexpr const unsigned time_check = Config::time_check_interval;

		run(const char*);

		run_error check(int& new_index) noexcept;
		void clear() noexcept;

		const char* path() const noexcept;
		job<Executor> const& running() const noexcept;
		bool is_running() const noexcept;
		int current() const noexcept;
	private:
		const char* 	path_;
		job<Executor>	job_;
		int				exec_idx_ = no_exec;
};

}//Jobs
}//Agro

#include "impl/job_impl.hpp"

#endif /* AGRO_MESH_SCHEDULER_HPP__ */
