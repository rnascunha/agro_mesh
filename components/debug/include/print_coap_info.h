#ifndef PRINT_COAP_INFO_H__
#define PRINT_COAP_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"
#include "coap.h"
//#include "coap_helper.h"

#ifdef CONFIG_PRINT_COAP_INFO_LOG_LEVEL

void print_uri(coap_uri_t* uri);
void print_uri_string(const char* uri_name);
void print_coap_pdu(coap_pdu_t* pdu, bool request);
void print_coap_transmission_param(coap_session_t* session);

#else

inline void __attribute__((always_inline)) print_uri(coap_uri_t* uri){}
inline void __attribute__((always_inline)) print_uri_string(const char* uri_name){}
inline void __attribute__((always_inline)) print_coap_pdu(coap_pdu_t* pdu, bool request){}
inline void __attribute__((always_inline)) print_coap_transmission_param(coap_session_t* session){}

#endif /* CONFIG_PRINT_COAP_INFO_LOG_LEVEL */

#ifdef __cplusplus
}
#endif


#endif /* PRINT_COAP_INFO_H__ */
