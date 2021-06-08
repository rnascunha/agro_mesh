#ifndef PRINT_WIFI_INFO_H__
#define PRINT_WIFI_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

#ifdef CONFIG_PRINT_WIFI_INFO_LOG_LEVEL

void print_wifi_bandwidth();
void print_wifi_max_tx_power();
void print_wifi_mac_addr();
void print_wifi_ap_records();
void print_wifi_country_info();
void print_wifi_channel();

#else

inline void __attribute__((always_inline)) print_wifi_bandwidth(){}
inline void __attribute__((always_inline)) print_wifi_max_tx_power(){}
inline void __attribute__((always_inline)) print_wifi_mac_addr(){}
inline void __attribute__((always_inline)) print_wifi_ap_records(){}
inline void __attribute__((always_inline)) print_wifi_country_info(){}
inline void __attribute__((always_inline)) print_wifi_channel(){}

#endif /* PRINT_WIFI_INFO_LOG_LEVEL */

#ifdef __cplusplus
}
#endif


#endif /* PRINT_WIFI_INFO_H__ */
