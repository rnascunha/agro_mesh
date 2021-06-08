
#include "wifi_station.h"
#include "print_system_info.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include <time.h>
#include <sys/time.h>

#include "esp_system.h"
#include "esp_event.h"
#include "esp_attr.h"

#include "esp_sntp.h"

static const char* TAG = "APP MAIN";

static void print_time_info(struct tm* time){
	printf("Time info: %02d:%02d:%02d %02d/%02d/%02d %d %d %d\n", time->tm_hour, time->tm_min, time->tm_sec,
			time->tm_mday, time->tm_mon, time->tm_year,
			time->tm_wday, time->tm_yday, time->tm_isdst);
}

static void print_time(time_t time){
	printf("Time: %ld\n", time);
}

static void print_timeval(struct timeval* tv){
	printf("Time val: %ld:%ld\n", tv->tv_sec, tv->tv_usec);
}

static void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
    print_timeval(tv);
}

extern "C" void app_main(void)
{
	print_system_reset_reason();

	ESP_LOGI(TAG, "Iniciando!");

	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);

	print_time(now);
	print_time_info(&timeinfo);

	wifi_basic_init();

	GET_WIFI_INSTANCE(wifi);
	wifi->init();

	wifi->wait_connect();

	char server[] = "pool.ntp.org";
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, server);
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();

    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    time(&now);
    localtime_r(&now, &timeinfo);

	print_time(now);
	print_time_info(&timeinfo);

	char strftime_buf[64];

	timer_t later = now;

	timeinfo.tm_hour = 10;
	timeinfo.tm_min = 40;
	timeinfo.tm_sec = 11;
	timeinfo.tm_mday = 30;
	timeinfo.tm_mon = 5 - 1;
	timeinfo.tm_year = 2019 - 1900;

	time_t time_made = mktime(&timeinfo);
	localtime_r(&time_made, &timeinfo);
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	ESP_LOGI(TAG, "Time made: %s", strftime_buf);

	while(true){
		time(&now);

		// Set timezone to Brazilia and print local time
		setenv("TZ", "<-03>3<-02>,M11.1.0/0,M2.3.0/0", 1);
		tzset();
		localtime_r(&now, &timeinfo);
		strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
		ESP_LOGI(TAG, "Brasilia: %s", strftime_buf);

		// Set timezone to Eastern Standard Time and print local time
		setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
		tzset();
		localtime_r(&now, &timeinfo);
		strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
		ESP_LOGI(TAG, "New York: %s", strftime_buf);

		// Set timezone to China Standard Time
		setenv("TZ", "CST-8", 1);
		tzset();
		localtime_r(&now, &timeinfo);
		strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
		ESP_LOGI(TAG, "Shanghai: %s", strftime_buf);

		time_t diff = now - later;
		localtime_r(&diff, &timeinfo);
		strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
		ESP_LOGI(TAG, "Diff: %ld / %s", diff, strftime_buf);

		localtime_r(&time_made, &timeinfo);
		strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
		ESP_LOGI(TAG, "Time made: %s\n", strftime_buf);

		vTaskDelay(20000 / portTICK_PERIOD_MS);
	}
}
