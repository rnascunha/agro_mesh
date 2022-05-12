#ifndef GET_TYPE_STRING_H__
#define GET_TYPE_STRING_H__

#include "esp_wifi.h"
//#include "esp_system.h"
#include "esp_chip_info.h"
//#include "ir.h"

#include <stdint.h>
#include "esp_event.h"
//#include "coap2/uri.h"

//#include "esp_partition.h"
//#include "esp_ota_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

const char* get_chip_model_string(esp_chip_model_t model);

const char* get_wifi_bandwidth_string(wifi_bandwidth_t band);
const char* get_wifi_max_tx_power_string(int8_t power);
const char* get_wifi_second_channel_string(wifi_second_chan_t ch);
const char* get_wifi_authmode_string(wifi_auth_mode_t mode);
const char* get_wifi_cipher_type_string(wifi_cipher_type_t cipher);
const char* get_wifi_antena_string(wifi_ant_t ant);
const char* get_wifi_contry_policy_string(wifi_country_policy_t policy);
const char* get_wifi_error_reason_string(wifi_err_reason_t err);

const char* get_system_reset_reason_string(esp_reset_reason_t reason);
//const char* get_ir_protocol_string(ir_protocol_t protocol);

//const char* get_coap_uri_scheme_string(enum coap_uri_scheme_t scheme);
//const char* get_coap_code_string(uint8_t code);
//const char* get_coap_type_message_string(uint8_t type);
//const char* get_coap_option_string(uint16_t opt);
//const char* get_coap_method_string(uint8_t method);
//const char* get_coap_media_type_string(uint16_t media);

//const char* get_partition_type_string(esp_partition_type_t type);
//const char* get_partition_subtype_string(esp_partition_subtype_t subtype);
//const char* get_ota_partition_state_string(esp_ota_img_states_t state);

#ifdef __cplusplus
}
#endif

#endif /* GET_TYPE_STRING_H__ */
