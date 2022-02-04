#ifndef AGRO_MESH_JOB_EXECUTOR_HPP__
#define AGRO_MESH_JOB_EXECUTOR_HPP__

#include <cstdio>

class Executor
{
	public:
		virtual bool read(FILE*) noexcept = 0;

		virtual void init() noexcept;
		virtual void finish() noexcept{};
		virtual void execute() noexcept = 0;

//		Executor& operator=(Executor&&) noexcept = 0;

		virtual ~Executor(){}
};

class Executor_Test : public Executor{
	public:
	static constexpr const std::size_t packet_size = 0;

		bool read(FILE*) noexcept;

		void init() noexcept override;
		void finish() noexcept override;
		void execute() noexcept override;

		Executor_Test& operator=(Executor_Test&&) noexcept;

		~Executor_Test(){}
};

#endif /* AGRO_MESH_JOB_EXECUTOR_HPP__ */
