#include "job.hpp"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include ".././net/coap_engine.hpp"
#include "../../net/messages/send.hpp"

#include "../../storage.hpp"

#define TAG		"JOB"

const char* job_path = STORAGE_BASE_PATH "/job";

extern engine coap_engine;

extern Agro::RTC_Time device_clock;
volatile bool job_force_check = false;

static void print_job(Agro::Jobs::job const& jb) noexcept
{
	DateTime dt;
	dt.setUnixTime(device_clock.get_local_time());

	printf("%02u/%02u/%04u %02u:%02u:%02u %d|%02u:%02u-%02u:%02u dow=%d/pri:%u/act:%u|%s\n",
//			device_clock.get_time(), device_clock.get_local_time(),
			dt.getDay(), dt.getMonth(), dt.getYear(),
			dt.getHour(), dt.getMinute(), dt.getSecond(),
			static_cast<int>(dt.dayOfWeek()),
			jb.sch.time_before.hour, jb.sch.time_before.minute,
			jb.sch.time_after.hour, jb.sch.time_after.minute,
			static_cast<int>(jb.sch.dw), jb.priority, jb.active,
			jb.sch.is_active(device_clock.get_local_time()) ? "active" : "not");
}

static void jobs(void*) noexcept
{
	ESP_LOGI(TAG, "Initiated job task...");
	Agro::Jobs::run scheduler{job_path};

	print_job(scheduler.running_job());
	do{
		int index;
		if(scheduler.check(index, job_force_check))
		{
			ESP_LOGI(TAG, "New job running");
			print_job(scheduler.running_job());
			CoAP::Error ec;
			char buf[30];
			snprintf(buf, 30, "new job running|%d", index);
			send_info(coap_engine, CoAP::Message::type::confirmable, buf, ec);
		}
		job_force_check = false;
		vTaskDelay((60 * 1000) / portTICK_RATE_MS);
	}while(true);
	vTaskDelete(NULL);
}

void init_job_task() noexcept
{
	if(storage_is_mounted())
	{
		xTaskCreate(&jobs, "jobs", 2048, NULL, 5, NULL);
	}
}
