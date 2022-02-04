#include "executor.hpp"

#include <cstring>
#include "app.hpp"
#include "loader.h"
#include "environment.hpp"
#include <malloc.h>

#define TASK_PRIORITY		5
#define TASK_STACK_SIZE		3072

Executor_App::Executor_App(){}

bool Executor_App::read(FILE* handler) noexcept
{
	std::size_t readed = fread(&arg_, 1, sizeof(int), handler);
	if(readed < sizeof(int)) return false;

	readed = fread(app_name_, 1, app_max_name_size, handler);
	if(readed < app_max_name_size) return false;

	app_name_[app_max_name_size] = '\0';

	return true;
}

void Executor_App::init() noexcept
{
	if(read_app_file(app_name_, &elf_) != app_status::success)
	{
		elf_ = nullptr;
	}
}

void Executor_App::finish() noexcept
{
	free(elf_);
	elf_ = nullptr;
}

void Executor_App::execute() noexcept
{
	if(elf_)
	{
		elfLoader(elf_, &env, "local_main", arg_);
	}
}

Executor_App& Executor_App::operator=(Executor_App&& rhs) noexcept
{
	arg_ = rhs.arg_;
	std::strcpy(app_name_, rhs.app_name_);
	elf_ = rhs.elf_;
	rhs.elf_ = nullptr;

	return *this;
}


