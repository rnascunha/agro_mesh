#ifndef AGRO_MESH_MODULES_JOB_DEFINES_HPP__
#define AGRO_MESH_MODULES_JOB_DEFINES_HPP__

#include "job.hpp"
//#include "../executor/executor.hpp"
#include "../app/executor.hpp"

namespace Agro{
namespace Jobs{

/**
 * bool Reexecute
 */
struct Job_Config{
	static constexpr const bool	reexecute = true;
	static constexpr const unsigned time_check_interval = 60 * 1000; //ms
};

using executor_type = Executor_App;
using job_type = Agro::Jobs::job<executor_type>;
using runner_type = Agro::Jobs::run<executor_type, Agro::Jobs::Job_Config>;

}//Agro
}//Jobs

#endif /* AGRO_MESH_MODULES_JOB_DEFINES_HPP__ */
