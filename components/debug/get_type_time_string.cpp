#include "get_type_time_string.h"

const char* get_scheduler_status_string(Scheduler::status_t status){
	switch(status){
		case Scheduler::NOT_CHECKED:
			return "Not checked";
		case Scheduler::NOT_RUNNING:
			return "Not running";
		case Scheduler::RUNNING:
			return "Running";
		case Scheduler::OUTDATED:
			return "Outdated";
		case Scheduler::SUSPEND:
			return "Suspended";
		case Scheduler::CANCEL:
			return "Canceled";
	}
	return "Undefined";
}
