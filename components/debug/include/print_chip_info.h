#ifndef PRINT_CHIP_INFO_H__
#define PRINT_CHIP_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

#ifdef CONFIG_PRINT_CHIP_INFO_LOG_LEVEL

void print_idf_version();
void print_chip_info();

#else

inline void __attribute__((always_inline)) print_idf_version(){}
inline void __attribute__((always_inline)) print_chip_info(){}

#endif /* CONFIG_PRINT_CHIP_INFO_LOG_LEVEL */

#ifdef __cplusplus
}
#endif

#endif /* PRINT_CHIP_INFO__ */
