#include "job.hpp"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "../../net/helper.hpp"
#include "../../storage.hpp"

#include "../clock/helper.hpp"
#include "job_define.hpp"

#define TAG		"JOB"

const char* job_path = STORAGE_BASE_PATH "/job";

Agro::Jobs::runner_type runner{job_path};

static void print_job(Agro::Jobs::job_type const& jb) noexcept;

static void jobs(void*) noexcept
{
	ESP_LOGI(TAG, "Initiated job task...");

	do{
		int index, index_b = runner.current();
		ESP_LOGD(TAG, "Checking jobs...");
		if(runner.check(index) == Agro::Jobs::run_error::success)
		{
			if(index_b != runner.current())
			{
				if(runner.current() == Agro::Jobs::runner_type::no_exec)
				{
					ESP_LOGI(TAG, "No job running");
				}
				else
				{
					ESP_LOGI(TAG, "New job running | %d\n", index);
					print_job(runner.running());
				}

				CoAP::Error ec;
				Agro::send_job_info(CoAP::Message::type::nonconfirmable, index, ec);
			}
		}
		vTaskDelay(Agro::Jobs::runner_type::time_check / portTICK_PERIOD_MS);
	}while(true);
	vTaskDelete(NULL);
}

namespace Agro{
namespace Jobs{

void init_task() noexcept
{
	if(storage_is_mounted())
	{
		xTaskCreate(&jobs, "jobs", 3072, NULL, 5, NULL);
	}
}

}//Jobs
}//Agro

static void print_job(Agro::Jobs::job_type const& jb) noexcept
{
	DateTime dt = Agro::get_local_datetime();

	printf("%02u:%02u:%02u %d|%02u:%02u-%02u:%02u dow=%d/pri:%u|%s\n",
			dt.getHour(), dt.getMinute(), dt.getSecond(),
			static_cast<int>(dt.dayOfWeek()),
			jb.sch.time_before.hour, jb.sch.time_before.minute,
			jb.sch.time_after.hour, jb.sch.time_after.minute,
			static_cast<int>(jb.sch.dw), jb.priority,
			jb.sch.is_active(Agro::get_local_time()) ? "active" : "not");
}
