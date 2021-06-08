#ifndef TIME_OPERATIONS_H__
#define TIME_OPERATIONS_H__

#include <time.h>
#include "sdkconfig.h"

int date_compare(const tm* date, const tm* date_cmp);

int date_between(const tm* date,
		const tm* date_before,
		const tm* date_after);

int time_compare(const tm* time, const tm* time_cmp);

int time_between(const tm* time,
		const tm* time_before,
		const tm* time_after);

#endif /* TIME_OPERATIONS_H__ */
