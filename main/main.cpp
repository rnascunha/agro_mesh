#include "nvs_flash.h"
#include "board/agro.hpp"
#include "sensor_pressure_task.hpp"
#include "agro/modules.hpp"

extern "C" void app_main(void)
{
	nvs_flash_init();

	Agro::Board::init();
	init_sensor_pressure_task();

	init_job_task();
}

