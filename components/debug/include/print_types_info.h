#ifndef PRINT_TYPES_INFO_H__
#define PRINT_TYPES_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

#ifdef CONFIG_PRINT_TYPES_INFO_LOG_LEVEL

void print_basic_types_size();

#else

inline void __attribute__((always_inline)) print_basic_types_size(){}

#endif /* CONFIG_PRINT_TYPES_INFO_LOG_LEVEL */

#ifdef __cplusplus
}
#endif

#endif
