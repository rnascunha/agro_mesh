#include "../job.hpp"
#include <utility>

#include "../../clock/helper.hpp"

namespace Agro{
namespace Jobs{

template<typename Executor>
void job<Executor>::execute() noexcept
{
	exec.execute();
}

template<typename Executor>
bool job<Executor>::read(FILE* handler) noexcept
{
	job_packet packet{};
	std::size_t readed = fread((void*)&packet, 1, sizeof(job_packet), handler);
	if(!readed || readed < sizeof(job_packet))
	{
		return false;
	}

	if(!exec.read(handler))
	{
		return false;
	}

	sch.time_before.hour = packet.be_time_hour;
	sch.time_before.minute = packet.be_time_minute;
	sch.time_after.hour = packet.af_time_hour;
	sch.time_after.minute = packet.af_time_minute;
	sch.dw = packet.day_of_week;
	priority = packet.priority;

	return true;
}

template<typename Executor>
void job<Executor>::start() noexcept
{
	exec.init();
}

template<typename Executor>
void job<Executor>::stop() noexcept
{
	exec.finish();
}

template<typename Executor>
job<Executor>& job<Executor>::operator=(job&& rhs) noexcept
{
	sch = rhs.sch;
	priority = rhs.priority;
	exec = std::move(rhs.exec);

	return *this;
}

template<typename Executor,
		typename Config>
run<Executor, Config>::run(const char* path) :
	path_(path){}

template<typename Executor,
		typename Config>
void run<Executor, Config>::clear() noexcept
{
	if(exec_idx_ != no_exec)
	{
		job_.stop();
	}
	exec_idx_ = no_exec;
}

template<typename Executor,
		typename Config>
const char* run<Executor, Config>::path() const noexcept
{
	return path_;
}

template<typename Executor,
		typename Config>
run_error
run<Executor, Config>::check(int& new_index) noexcept
{
	value_time time = get_local_time();

	new_index = no_exec;
	/**
	 * Checking if current job is still valid
	 */
	if(exec_idx_ != no_exec && !job_.sch.is_active(time))
	{
		job_.stop();
		exec_idx_ = no_exec;
	}

	/**
	 * Opening job files
	 */
	FILE *f = fopen(path_, "rb");
	if(!f)
	{
		clear();
		return run_error::file_open;
	}

	int index = no_exec, nindex = no_exec;
	std::uint8_t curr_priority = exec_idx_ == no_exec ? 0 : job_.priority;
	job<Executor> njob, sel_job;
	do{
		if(!njob.read(f))
		{
			break;
		}
		index++;

		if(njob.sch.is_active(time) &&
			njob.priority > curr_priority)
		{
			nindex = index;
			curr_priority = njob.priority;
			sel_job = std::move(njob);
		}
	}while(true);
	fclose(f);

	if(nindex != no_exec && exec_idx_ != nindex)
	{
		job_.stop();
		job_ = std::move(sel_job);
		job_.start();
		job_.execute();
		new_index = nindex;
		exec_idx_ = nindex;
	}
	else
	{
		if constexpr(Config::reexecute)
		{
			if(exec_idx_ != no_exec)
			{
				job_.execute();
			}
		}
	}

	return run_error::success;
}

template<typename Executor,
		typename Config>
job<Executor> const&
run<Executor, Config>::running() const noexcept
{
	return job_;
}

template<typename Executor,
		typename Config>
bool
run<Executor, Config>::is_running() const noexcept
{
	return exec_idx_ != no_exec;
}

template<typename Executor,
		typename Config>
int
run<Executor, Config>::current() const noexcept
{
	return exec_idx_;
}

}//Jobs
}//Agro
