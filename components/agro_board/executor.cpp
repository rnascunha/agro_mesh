#include "executor.hpp"
#include "gpio.h"

extern GPIO_Basic ac_load[];
extern const std::size_t ac_load_count;

Executor_Output::Executor_Output(std::uint8_t active)
	: active_(active){}

void Executor_Output::execute() noexcept
{
	for(std::size_t i = 0; i < ac_load_count; i++)
	{
		ac_load[i].write(active_ & (1 << i) ? 1 : 0);
	}
}

