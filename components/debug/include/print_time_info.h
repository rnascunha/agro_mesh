#ifndef PRINT_TIME_INFO_H__
#define PRINT_TIME_INFO_H__

#include <time.h>
#include <sys/time.h>

#include "scheduler.h"
#include "run_scheduler.h"

#include "sdkconfig.h"

#ifdef CONFIG_PRINT_TIME_INFO_LOG_LEVEL

void print_time(const struct tm* time);
void print_time(const struct timeval* tv);
void print_time(const time_t* time, const char* format = CONFIG_DEFAULT_TIME_REPRESATITION );
void print_time_now(const char* format = CONFIG_DEFAULT_TIME_REPRESATITION);

void print_scheduler(Scheduler* sch);
void print_run_scheduler(Run_Scheduler* sch);
#else

inline void __attribute__((always_inline)) print_time(const struct tm* time){}
inline void __attribute__((always_inline)) print_time(const struct timeval* tv){}
inline void __attribute__((always_inline)) print_time(const time_t* time, const char* format = CONFIG_DEFAULT_TIME_REPRESATITION ){}
inline void __attribute__((always_inline)) print_time_now(const char* format = CONFIG_DEFAULT_TIME_REPRESATITION){}

inline void __attribute__((always_inline)) print_scheduler(Scheduler* sch){}
inline void __attribute__((always_inline)) print_run_scheduler(Run_Scheduler* sch){}

#endif /* CONFIG_PRINT_TIME_INFO_LOG_LEVEL */

#endif /* PRINT_TIME_INFO_H__ */
