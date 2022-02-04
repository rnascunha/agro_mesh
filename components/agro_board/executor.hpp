#include "agro/modules.hpp"

#include <cstdint>

class Executor_Output : public Executor{
	public:
		Executor_Output(std::uint8_t);

		virtual void execute() noexcept override;
	public:
		std::uint8_t active_;
};
