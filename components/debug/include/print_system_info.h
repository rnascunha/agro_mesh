#ifndef PRINT_SYSTEM_INFO_H__
#define PRINT_SYSTEM_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

#ifdef CONFIG_PRINT_SYSTEM_INFO_LOG_LEVEL

void print_system_reset_reason();
void print_base_mac_addr();

#else

inline void __attribute__((always_inline)) print_system_reset_reason(){}
inline void __attribute__((always_inline)) print_base_mac_addr(){}

#endif /* CONFIG_PRINT_SYSTEM_INFO_LOG_LEVEL */

#ifdef __cplusplus
}
#endif

#endif /* PRINT_SYSTEM_INFO_H__ */
