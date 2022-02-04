#include "executor.hpp"

bool Executor_Test::read(FILE*) noexcept
{
	std::printf("Executor read...\n");
	return true;
}

void Executor_Test::init() noexcept
{
	std::printf("Executor initiated\n");
}

void Executor_Test::finish() noexcept
{
	std::printf("Executor finalized\n");
}

void Executor_Test::execute() noexcept
{
	std::printf("Executing...\n");
}

Executor_Test& Executor_Test::operator=(Executor_Test&&) noexcept
{
	std::printf("Executor move...\n");
	return *this;
}
