#include "print_time_info.h"
#include <stdio.h>
#include "get_type_time_string.h"

void print_time(const struct tm* time){
	printf("Time info: %02d:%02d:%02d %02d/%02d/%02d %d %d %d\n", time->tm_hour, time->tm_min, time->tm_sec,
			time->tm_mday, time->tm_mon, time->tm_year,
			time->tm_wday, time->tm_yday, time->tm_isdst);
}

void print_time(const struct timeval* tv){
	printf("Time val: %ld,%ld\n", tv->tv_sec, tv->tv_usec);
}

void print_time(const time_t* time, const char* format /* = "%c" */){
	char strftime_buf[64];

	struct tm timeinfo;
	localtime_r(time, &timeinfo);
	strftime(strftime_buf, sizeof(strftime_buf), format, &timeinfo);
	printf("%s", strftime_buf);
}

void print_time_now(const char* format){
	time_t now;
	time(&now);
	print_time(&now, format);
}

void print_scheduler(Scheduler* sch){
	time_t time;
	printf("%s[%s]:", sch->get_name(), get_scheduler_status_string(sch->get_status()));
	time = sch->get_time0();
	print_time(&time);
	printf("|");
	time = sch->get_time1();
	print_time(&time);
	printf("\n");
}

void print_run_scheduler(Run_Scheduler* sch){
	Scheduler* item = NULL;

	printf("Schedulers: %u\n", sch->list_sch.get_number());
	for(unsigned int i = 0; sch->list_sch.get_item(i, item) >= 0; i++){
		print_scheduler(item);
	}
}

