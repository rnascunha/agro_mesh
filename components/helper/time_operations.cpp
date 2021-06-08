#include "time_operations.h"

int date_compare(const tm* date, const tm* date_cmp){
	if(date->tm_year < date_cmp->tm_year){
		return -1;
	}
	if(date->tm_year > date_cmp->tm_year){
		return 1;
	}
	if(date->tm_mon < date_cmp->tm_mon){
		return -1;
	}
	if(date->tm_mon > date_cmp->tm_mon){
		return 1;
	}
	if(date->tm_mday < date_cmp->tm_mday){
		return -1;
	}
	if(date->tm_mday > date_cmp->tm_mday){
		return 1;
	}

	return 0;
}

int date_between(const tm* date,
		const tm* date_before,
		const tm* date_after){

	int date_cmp0 = date_compare(date, date_before);
	int date_cmp1 = date_compare(date, date_after);

	if(date_cmp0 < 0){
		return -1;
	} else if(date_cmp1 > 0){
		return 1;
	}

	return 0;
}

int time_compare(const tm* time, const tm* time_cmp){
	if(time->tm_hour < time_cmp->tm_hour){
		return -1;
	}
	if(time->tm_hour > time_cmp->tm_hour){
		return 1;
	}
	if(time->tm_min < time_cmp->tm_min){
		return -1;
	}
	if(time->tm_min > time_cmp->tm_min){
		return 1;
	}
	if(time->tm_sec < time_cmp->tm_sec){
		return -1;
	}
	if(time->tm_sec > time_cmp->tm_sec){
		return 1;
	}

	return 0;
}

int time_between(const tm* time,
		const tm* time_before,
		const tm* time_after){

	int time_cmp0 = time_compare(time, time_before);
	int time_cmp1 = time_compare(time, time_after);

	if(time_cmp0 < 0){
		return -1;
	} else if(time_cmp1 > 0){
		return 1;
	}

	return 0;
}
