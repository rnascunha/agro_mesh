#ifndef AGRO_MESH_MODULE_APP_EXECUTOR_HPP__
#define AGRO_MESH_MODULE_APP_EXECUTOR_HPP__

#include "app.hpp"
#include "../executor/executor.hpp"

class Executor_App : public Executor{
	public:
		static constexpr const std::size_t packet_size = sizeof(int) + app_max_name_size;

		Executor_App();

		bool read(FILE*) noexcept override;

		virtual void init() noexcept override;
		virtual void finish() noexcept override;
		virtual void execute() noexcept override;

		Executor_App& operator=(Executor_App&&) noexcept;

		~Executor_App();
	private:
		int 			arg_ = 0;
		unsigned char*	elf_ = nullptr;
		char 			app_name_[app_max_name_size + 1];
};

#endif /* AGRO_MESH_MODULE_APP_EXECUTOR_HPP__ */
