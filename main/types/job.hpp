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
	bool operator>=(time const&) const noexcept;
	bool operator<=(time const&) const noexcept;

	time& operator=(time const&) noexcept = default;
};

bool between(value_time, time const& before, time const& after) noexcept;

struct scheduler{
	scheduler() = default;
	scheduler(scheduler const&) =  default;
	scheduler(time const& before, time const& after, dow dw_ = all_dow_days);

	time			time_before;
	time			time_after;
	dow				dw = all_dow_days;		///< Day of the week

	bool operator==(scheduler const&) const noexcept;
	scheduler& operator=(scheduler const&) noexcept = default;
	bool is_active(value_time) const noexcept;
	bool operator()() const noexcept;
};

void execute(std::uint8_t act) noexcept;

struct job{
	static constexpr const std::size_t packet_size = 7;

	job() = default;
	job(scheduler const& sch_, std::uint8_t prior, std::uint8_t act)
		: sch(sch_), priority(prior), active(act){}

	scheduler		sch;
	std::uint8_t	priority = 0;
	std::uint8_t	active = 0;

	job& operator=(job const&) noexcept = default;
	bool operator==(job const&) noexcept;
	void execute() const noexcept;
};

class run{
	public:
		run(const char*);

		bool check(bool force = false) noexcept;

		/**
		 *
		 * @param index Index of new job running (if return is true)
		 * @return Check if new job is running
		 * @retval true new job is running
		 * @retval false old job still running
		 */
		bool check(int& index, bool force = false) noexcept;
		job const& running_job() const noexcept{ return job_; }
	private:
		void clear() noexcept;
		bool is_clear() const noexcept;

		const char* path_;
		job			job_;
};

}//Jobs

#endif /* AGRO_MESH_SCHEDULER_HPP__ */
