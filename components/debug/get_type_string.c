#include "get_type_string.h"
//#include "coap2/uri.h"
//#include "coap.h"

const char* get_wifi_bandwidth_string(wifi_bandwidth_t band){
	return band == WIFI_BW_HT20 ? "HT20" : (band == WIFI_BW_HT40 ? "HT40" : "Undefined");
}

const char* get_chip_model_string(esp_chip_model_t model){
	return model == CHIP_ESP32 ? "ESP32" : "Undefined";
}

const char* get_wifi_max_tx_power_string(int8_t power){
	switch(power){
		case 78:
			return "19.5";
		case 76:
			return "19";
		case 74:
			return "18.5";
		case 68:
			return "17";
		case 60:
			return "15";
		case 52:
			return "13";
		case 44:
			return "11";
		case 34:
			return "8.5";
		case 28:
			return "7";
		case 20:
			return "5";
		case 8:
			return "2";
		case -4:
			return "-1";
	}
	return "Udenfiend";
}

const char* get_wifi_second_channel_string(wifi_second_chan_t ch){
	switch(ch){
		case WIFI_SECOND_CHAN_NONE:
			return "NONE";
		case WIFI_SECOND_CHAN_ABOVE:
			return "ABOVE";
		case WIFI_SECOND_CHAN_BELOW:
			return "BELOW";
	}
	return "Undefined";
}

const char* get_wifi_authmode_string(wifi_auth_mode_t mode){
	switch(mode){
		case WIFI_AUTH_OPEN:         /**< authenticate mode : open */
			return "OPEN";
		case WIFI_AUTH_WEP:              /**< authenticate mode : WEP */
			return "WEP";
		case WIFI_AUTH_WPA_PSK:          /**< authenticate mode : WPA_PSK */
			return "WPA_PSK";
		case WIFI_AUTH_WPA2_PSK:         /**< authenticate mode : WPA2_PSK */
			return "WPA2_PSK";
		case WIFI_AUTH_WPA_WPA2_PSK:     /**< authenticate mode : WPA_WPA2_PSK */
			return "WPA_WPA2_PSK";
		case WIFI_AUTH_WPA2_ENTERPRISE:  /**< authenticate mode : WPA2_ENTERPRISE */
			return "WPA2_ENTERPRISE";
		case WIFI_AUTH_MAX:
			return "MAX";
		default:
			break;
	}
	return "Undefined";
}

const char* get_wifi_cipher_type_string(wifi_cipher_type_t cipher){
	switch(cipher){
		case WIFI_CIPHER_TYPE_NONE:   /**< the cipher type is none */
			return "NONE";
		case WIFI_CIPHER_TYPE_WEP40:      /**< the cipher type is WEP40 */
			return "WEP40";
		case WIFI_CIPHER_TYPE_WEP104:     /**< the cipher type is WEP104 */
			return "WEB104";
		case WIFI_CIPHER_TYPE_TKIP:       /**< the cipher type is TKIP */
			return "TKIP";
		case WIFI_CIPHER_TYPE_CCMP:       /**< the cipher type is CCMP */
			return "CCMP";
		case WIFI_CIPHER_TYPE_TKIP_CCMP:  /**< the cipher type is TKIP and CCMP */
			return "TKIP_CCMP";
		case WIFI_CIPHER_TYPE_UNKNOWN:    /**< the cipher type is unknown */
			return "UNKNOW";
		default:
			break;
	}
	return "Undefined";
}

const char* get_wifi_antena_string(wifi_ant_t ant){
	switch(ant){
		case WIFI_ANT_ANT0:          /**< WiFi antenna 0 */
			return "ANT0";
		case WIFI_ANT_ANT1:          /**< WiFi antenna 1 */
			return "ANT1";
		case WIFI_ANT_MAX:           /**< Invalid WiFi antenna */
			return "MAX";
	}
	return "Undefined";
}

const char* get_wifi_contry_policy_string(wifi_country_policy_t policy){
	switch(policy){
		case WIFI_COUNTRY_POLICY_AUTO:   /**< Country policy is auto, use the country info of AP to which the station is connected */
			return "AUTO";
		case WIFI_COUNTRY_POLICY_MANUAL: /**< Country policy is manual, always use the configured country info */
			return "MANUAL";
	}
	return "Undefined";
}

const char* get_wifi_error_reason_string(wifi_err_reason_t err){
	switch(err){
	case WIFI_REASON_UNSPECIFIED:
		return "UNSPECIFIED";
	case WIFI_REASON_AUTH_EXPIRE:
		return "AUTH_EXPIRE";
	case WIFI_REASON_AUTH_LEAVE:
		return "AUTH_LEAVE";
	case WIFI_REASON_ASSOC_EXPIRE:
		return "ASSOC_EXPIRE";
	case WIFI_REASON_ASSOC_TOOMANY:
		return "ASSOC_TOOMANY";
	case WIFI_REASON_NOT_AUTHED:
		return "NOT_AUTHED";
	case WIFI_REASON_NOT_ASSOCED:
		return "NOT_ASSOCED";
	case WIFI_REASON_ASSOC_LEAVE:
		return "ASSOC_LEAVE";
	case WIFI_REASON_ASSOC_NOT_AUTHED:
		return "ASSOC_NOT_AUTHED";
	case WIFI_REASON_DISASSOC_PWRCAP_BAD:
		return "DISASSOC_PWRCAP_BAD";
	case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
		return "DISASSOC_SUPCHAN_BAD";
	case WIFI_REASON_IE_INVALID:
		return "IE_INVALID";
	case WIFI_REASON_MIC_FAILURE:
		return "MIC_FAILURE";
	case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
		return "4WAY_HANDSHAKE_TIMEOUT";
	case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
		return "GROUP_KEY_UPDATE_TIMEOUT";
	case WIFI_REASON_IE_IN_4WAY_DIFFERS:
		return "IE_IN_4WAY_DIFFERS";
	case WIFI_REASON_GROUP_CIPHER_INVALID:
		return "GROUP_CIPHER_INVALID";
	case WIFI_REASON_PAIRWISE_CIPHER_INVALID:
		return "PAIRWISE_CIPHER_INVALID";
	case WIFI_REASON_AKMP_INVALID:
		return "AKMP_INVALID";
	case WIFI_REASON_UNSUPP_RSN_IE_VERSION:
		return "UNSUPP_RSN_IE_VERSION";
	case WIFI_REASON_INVALID_RSN_IE_CAP:
		return "INVALID_RSN_IE_CAP";
	case WIFI_REASON_802_1X_AUTH_FAILED:
		return "802_1X_AUTH_FAILED";
	case WIFI_REASON_CIPHER_SUITE_REJECTED:
		return "CIPHER_SUITE_REJECTED";
	case WIFI_REASON_BEACON_TIMEOUT:
		return "BEACON_TIMEOUT";
	case WIFI_REASON_NO_AP_FOUND:
		return "NO_AP_FOUND";
	case WIFI_REASON_AUTH_FAIL:
		return "AUTH_FAIL";
	case WIFI_REASON_ASSOC_FAIL:
		return "ASSOC_FAIL";
	case WIFI_REASON_HANDSHAKE_TIMEOUT:
		return "HANDSHAKE_TIMEOUT";
	case WIFI_REASON_CONNECTION_FAIL:
		return "CONNECTION_FAIL";
	default:
		break;
	}

	return "Undefined";
}

const char* get_system_reset_reason_string(esp_reset_reason_t reason){
    switch(reason){
    	case ESP_RST_UNKNOWN:    //!< Reset reason can not be determined
    		return "UNKNOW";
		case ESP_RST_POWERON:    //!< Reset due to power-on event
			return "POWERON";
		case ESP_RST_EXT:        //!< Reset by external pin (not applicable for ESP32)
			return "EXTERNAL";
		case ESP_RST_SW:         //!< Software reset via esp_restart
			return "SOFTWARE";
		case ESP_RST_PANIC:      //!< Software reset due to exception/panic
			return "PANIC";
		case ESP_RST_INT_WDT:    //!< Reset (software or hardware) due to interrupt watchdog
			return "INTERRUPT WDT";
		case ESP_RST_TASK_WDT:   //!< Reset due to task watchdog
			return "TASK WDT";
		case ESP_RST_WDT:        //!< Reset due to other watchdogs
			return "WDT";
		case ESP_RST_DEEPSLEEP:  //!< Reset after exiting deep sleep mode
			return "DEEP SLEEP";
		case ESP_RST_BROWNOUT:   //!< Brownout reset (software or hardware)
			return "BROWNOUT";
		case ESP_RST_SDIO:       //!< Reset over SDIO
			return "SDIO";
    }
	return "Undefined";
}

//const char* get_ir_protocol_string(ir_protocol_t protocol){
//	switch(protocol){
//		case IR_NEC:
//			return "NEC";
//		case IR_NEC_EXT:
//			return "NEC_EXT";
//		case IR_NEC_REPEAT:
//			return "NEC_REPEAT";
//		case IR_GENERIC:
//			return "GENERIC";
//		case IR_UNKNOWN:
//			return "UNKNOWN";
//	}
//	return "Undefined";
//}

//const char* get_coap_uri_scheme_string(enum coap_uri_scheme_t scheme){
//	switch(scheme){
//		case COAP_URI_SCHEME_COAP:
//			return "CoAP";
//		case COAP_URI_SCHEME_COAPS:
//			return "CoAPS";
//		case COAP_URI_SCHEME_COAP_TCP:
//			return "CoAP+TCP";
//		case COAP_URI_SCHEME_COAPS_TCP:
//			return "CoAPS+TCP";
//	}
//	return "Undefined";
//}
//
//const char* get_coap_type_message_string(uint8_t type){
//	switch(type){
//		case COAP_MESSAGE_CON:
//			return "Confirmable";
//		case COAP_MESSAGE_NON:
//			return "Non-confirmable";
//		case COAP_MESSAGE_ACK:
//			return "Acknowledgement";
//		case COAP_MESSAGE_RST:
//			return "Reset";
//	//		COAP_MESSAGE_SIZE_OFFSET_TCP16
//	//		COAP_MESSAGE_SIZE_OFFSET_TCP16
//	//		COAP_MESSAGE_SIZE_OFFSET_TCP32
//	}
//	return "Undefined";
//}
//
//const char* get_coap_option_string(uint16_t opt){
//	switch(opt){
//		case COAP_OPTION_IF_MATCH:			//   1 /* C, opaque, 0-8 B, (none) */
//			return "If-Match";
//		case COAP_OPTION_URI_HOST:			//   3 /* C, String, 1-255 B, destination address */
//			return "Uri Host";
//		case COAP_OPTION_ETAG:				//   4 /* E, opaque, 1-8 B, (none) */
//			return "Etag";
//		case COAP_OPTION_IF_NONE_MATCH:		//   5 /* empty, 0 B, (none) */
//			return "If-None-Match";
//		case COAP_OPTION_URI_PORT:			//   7 /* C, uint, 0-2 B, destination port */
//			return "Uri port";
//		case COAP_OPTION_LOCATION_PATH:		//   8 /* E, String, 0-255 B, - */
//			return "Location Path";
//		case COAP_OPTION_URI_PATH:			//   11 /* C, String, 0-255 B, (none) */
//			return "Uri Path";
//		case COAP_OPTION_CONTENT_FORMAT:	//   12 /* E, uint, 0-2 B, (none) */
//			return "Content Format/Type";
////		case COAP_OPTION_CONTENT_TYPE:		//   COAP_OPTION_CONTENT_FORMAT
////			return "Content Type";
//		case COAP_OPTION_MAXAGE:			//   14 /* E, uint, 0--4 B, 60 Seconds */
//			return "Max Age";
//		case COAP_OPTION_URI_QUERY:			//   15 /* C, String, 1-255 B, (none) */
//			return "Uri Query";
//		case COAP_OPTION_ACCEPT:			//   17 /* C, uint, 0-2 B, (none) */
//			return "Accpet";
//		case COAP_OPTION_LOCATION_QUERY:	//   20 /* E, String, 0-255 B, (none) */
//			return "Location Query";
//		case COAP_OPTION_SIZE2:				//   28 /* E, uint, 0-4 B, (none) */
//			return "Size2";
//		case COAP_OPTION_PROXY_URI:			//   35 /* C, String, 1-1034 B, (none) */
//			return "Proxy Uri";
//		case COAP_OPTION_PROXY_SCHEME:		//   39 /* C, String, 1-255 B, (none) */
//			return "Proxy Scheme";
//		case COAP_OPTION_SIZE1:				//   60 /* E, uint, 0-4 B, (none) */
//			return "Size1";
//		case COAP_OPTION_OBSERVE:			//   6 /* E, empty/uint, 0 B/0-3 B, (none) */
//			return "Observe";
////		case COAP_OPTION_SUBSCRIPTION:		//   COAP_OPTION_OBSERVE
//		case COAP_OPTION_BLOCK2:			//   23 /* C, uint, 0--3 B, (none) */
//			return "Block2";
//		case COAP_OPTION_BLOCK1:			//   27 /* C, uint, 0--3 B, (none) */
//			return "Block1";
//		case COAP_OPTION_NORESPONSE:		//   258 /* N, uint, 0--1 B, 0 */
//			return "No Response";
//	}
//
//	return "Undefined";
//}
//
//const char* get_coap_code_string(uint8_t code){
//	switch(COAP_RESPONSE_CODE(code)){
////		case COAP_RESPONSE_CODE(000):
////			return "0.00 Empty Message";
//		case COAP_RESPONSE_CODE(200):
//			return "2.00 Success";
//		case COAP_RESPONSE_CODE(201):
//			return "2.01 Created";
//		case COAP_RESPONSE_CODE(202):
//			return "2.02 Deleted";
//		case COAP_RESPONSE_CODE(203):
//			return "2.03 Valid";
//		case COAP_RESPONSE_CODE(204):
//			return "2.04 Changed";
//		case COAP_RESPONSE_CODE(205):
//			return "2.05 Content";
//		case COAP_RESPONSE_CODE(231):
//			return "2.31 Continue";
//		case COAP_RESPONSE_CODE(400):
//			return "4.00 Bad Request";
//		case COAP_RESPONSE_CODE(401):
//			return "4.01 Unauthorized";
//		case COAP_RESPONSE_CODE(402):
//			return "4.02 Bad Option";
//		case COAP_RESPONSE_CODE(403):
//			return "4.03 Forbidden";
//		case COAP_RESPONSE_CODE(404):
//			return "4.04 Not Found";
//		case COAP_RESPONSE_CODE(405):
//			return "4.05 Method Not Allowed";
//		case COAP_RESPONSE_CODE(406):
//			return "4.06 Not Acceptable";
//		case COAP_RESPONSE_CODE(408):
//			return "4.08 Request Entity Incomplete";
//		case COAP_RESPONSE_CODE(412):
//			return "4.12 Precondition Failed";
//		case COAP_RESPONSE_CODE(413):
//			return "4.13 Request Entity Too Large";
//		case COAP_RESPONSE_CODE(415):
//			return "4.15 Unsupported Media Type";
//		case COAP_RESPONSE_CODE(500):
//			return "5.00 Internal Server Error";
//		case COAP_RESPONSE_CODE(501):
//			return "5.01 Not Implemented";
//		case COAP_RESPONSE_CODE(503):
//			return "5.03 Service Unavailable";
//		case COAP_RESPONSE_CODE(504):
//			return "5.04 Gateway Timeout";
//		case COAP_RESPONSE_CODE(505):
//			return "5.05 Proxying Not Supported";
//		default:
//			break;
//	}
//	return "Undefined";
//}
//
//const char* get_coap_method_string(uint8_t method){
//	switch(method){
//		case COAP_REQUEST_GET:
//			return "Get";
//		case COAP_REQUEST_POST:
//			return "Post";
//		case COAP_REQUEST_PUT:
//			return "Put";
//		case COAP_REQUEST_DELETE:
//			return "Delete";
//		case COAP_REQUEST_FETCH:
//			return "Fetch";
//		case COAP_REQUEST_PATCH:
//			return "Patch";
//		case COAP_REQUEST_IPATCH:
//			return "IPatch";
//	}
//	return "Undefined";
//}
//
//const char* get_coap_media_type_string(uint16_t media){
//	switch(media){
//	/* CoAP media type encoding */
//		case COAP_MEDIATYPE_TEXT_PLAIN:					//   0 /* text/plain (UTF-8) */
//			return "Text Plain";
//		case COAP_MEDIATYPE_APPLICATION_LINK_FORMAT:	//   40 /* application/link-format */
//			return "Link Format";
//		case COAP_MEDIATYPE_APPLICATION_XML:			//   41 /* application/xml */
//			return "XML";
//		case COAP_MEDIATYPE_APPLICATION_OCTET_STREAM:	//   42 /* application/octet-stream */
//			return "Octet Stream";
//		case COAP_MEDIATYPE_APPLICATION_RDF_XML:		//   43 /* application/rdf+xml */
//			return "RDF XML";
//		case COAP_MEDIATYPE_APPLICATION_EXI:			//   47 /* application/exi  */
//			return "EXI";
//		case COAP_MEDIATYPE_APPLICATION_JSON:			//   50 /* application/json  */
//			return "JSON";
//		case COAP_MEDIATYPE_APPLICATION_CBOR:			//   60 /* application/cbor  */
//			return "CBOR";
//
//	/* Content formats from RFC 8152 */
//		case COAP_MEDIATYPE_APPLICATION_COSE_SIGN:		//   98 /* application/cose; cose-type="cose-sign"     */
//			return "Cose Sign";
//		case COAP_MEDIATYPE_APPLICATION_COSE_SIGN1:		//   18 /* application/cose; cose-type="cose-sign1"    */
//			return "Cose Sign1";
//		case COAP_MEDIATYPE_APPLICATION_COSE_ENCRYPT:	//   96 /* application/cose; cose-type="cose-encrypt"  */
//			return "Cose Encrypt";
//		case COAP_MEDIATYPE_APPLICATION_COSE_ENCRYPT0:	//   16 /* application/cose; cose-type="cose-encrypt0" */
//			return "Cose Encrypt0";
//		case COAP_MEDIATYPE_APPLICATION_COSE_MAC:		//   97 /* application/cose; cose-type="cose-mac"      */
//			return "Cose Mac";
//		case COAP_MEDIATYPE_APPLICATION_COSE_MAC0:		//   17 /* application/cose; cose-type="cose-mac0"     */
//			return "Cose Mac0";
//
//		case COAP_MEDIATYPE_APPLICATION_COSE_KEY:		//   101 /* application/cose-key  */
//			return "Cose Key";
//		case COAP_MEDIATYPE_APPLICATION_COSE_KEY_SET:	//   102 /* application/cose-key-set  */
//			return "Cose Key Set";
//
//	/* Content formats from RFC 8428 */
//		case COAP_MEDIATYPE_APPLICATION_SENML_JSON:		//   110 /* application/senml+json  */
//			return "SENML JSON";
//		case COAP_MEDIATYPE_APPLICATION_SENSML_JSON:	//   111 /* application/sensml+json */
//			return "SENSML JSON";
//		case COAP_MEDIATYPE_APPLICATION_SENML_CBOR:		//   112 /* application/senml+cbor  */
//			return "SENML CBOR";
//		case COAP_MEDIATYPE_APPLICATION_SENSML_CBOR:	//   113 /* application/sensml+cbor */
//			return "SENSML CBOR";
//		case COAP_MEDIATYPE_APPLICATION_SENML_EXI:		//   114 /* application/senml-exi   */
//			return "SENML EXI";
//		case COAP_MEDIATYPE_APPLICATION_SENSML_EXI:		//   115 /* application/sensml-exi  */
//			return "SENSML EXI";
//		case COAP_MEDIATYPE_APPLICATION_SENML_XML:		//   310 /* application/senml+xml   */
//			return "SENML XML";
//		case COAP_MEDIATYPE_APPLICATION_SENSML_XML:		//   311 /* application/sensml+xml  */
//			return "SENSML XML";
//	}
//	return "Undefined";
//}

//const char* get_partition_type_string(esp_partition_type_t type){
//	switch(type){
//		case ESP_PARTITION_TYPE_APP:
//			return "App";
//		case ESP_PARTITION_TYPE_DATA:
//			return "Data";
//	}
//	return "Undefined";
//}
//
//const char* get_partition_subtype_string(esp_partition_subtype_t subtype){
//	switch(subtype){
//		case ESP_PARTITION_SUBTYPE_APP_FACTORY:          //!< Factory application partition
//			return "Factory";
////		case ESP_PARTITION_SUBTYPE_APP_OTA_MIN:	//!< Base for OTA partition subtypes
//		case ESP_PARTITION_SUBTYPE_APP_OTA_0:	//!< OTA partition 0
//			return "Ota 0";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_1:	//!< OTA partition 1
//			return "Ota 1";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_2:	//!< OTA partition 2
//			return "Ota 2";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_3:	//!< OTA partition 3
//			return "Ota 3";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_4:	//!< OTA partition 4
//			return "Ota 4";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_5:	//!< OTA partition 5
//			return "Ota 5";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_6:	 //!< OTA partition 6
//			return "Ota 6";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_7:	//!< OTA partition 7
//			return "Ota 7";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_8:	//!< OTA partition 8
//			return "Ota 8";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_9:	//!< OTA partition 9
//			return "Ota 9";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_10:	//!< OTA partition 10
//			return "Ota 10";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_11:	//!< OTA partition 11
//			return "Ota 11";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_12:	//!< OTA partition 12
//			return "Ota 12";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_13:	//!< OTA partition 13
//			return "Ota 13";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_14:	//!< OTA partition 14
//			return "Ota 14";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_15:	//!< OTA partition 15
//			return "Ota 15";
//		case ESP_PARTITION_SUBTYPE_APP_OTA_MAX:									//!< Max subtype of OTA partition
//			return "Ota Max";
////		case ESP_PARTITION_SUBTYPE_APP_TEST:                                    //!< Test application partition
////			return "Test";
////		case ESP_PARTITION_SUBTYPE_DATA_OTA:                                    //!< OTA selection partition
////			return "Ota";
//		case ESP_PARTITION_SUBTYPE_DATA_PHY:                                    //!< PHY init data partition
//			return "PHY";
//		case ESP_PARTITION_SUBTYPE_DATA_NVS:                                    //!< NVS partition
//			return "NVS";
//		case ESP_PARTITION_SUBTYPE_DATA_COREDUMP:                               //!< COREDUMP partition
//			return "Core Dump";
//		case ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS:                               //!< Partition for NVS keys
//			return "NVS keys";
//		case ESP_PARTITION_SUBTYPE_DATA_EFUSE_EM:                              //!< Partition for emulate eFuse bits
//			return "Efuse EM";
//		case ESP_PARTITION_SUBTYPE_DATA_ESPHTTPD:                               //!< ESPHTTPD partition
//			return "ESP HTTPD";
//		case ESP_PARTITION_SUBTYPE_DATA_FAT:                                    //!< FAT partition
//			return "FAT";
//		case ESP_PARTITION_SUBTYPE_DATA_SPIFFS:                             //!< SPIFFS partition
//			return "SPIFFS";
//		case ESP_PARTITION_SUBTYPE_ANY:
//			return "Any";
//	}
//	return "Undefined";
//}
//
//const char* get_ota_partition_state_string(esp_ota_img_states_t state){
//	switch(state){
//		case ESP_OTA_IMG_NEW://             = 0x0U,         /*!< Monitor the first boot. In bootloader this state is changed to ESP_OTA_IMG_PENDING_VERIFY. */
//			return "New";
//		case ESP_OTA_IMG_PENDING_VERIFY://  = 0x1U,         /*!< First boot for this app was. If while the second boot this state is then it will be changed to ABORTED. */
//			return "Pending Verify";
//		case ESP_OTA_IMG_VALID://           = 0x2U,         /*!< App was confirmed as workable. App can boot and work without limits. */
//			return "Valid";
//		case ESP_OTA_IMG_INVALID://         = 0x3U,         /*!< App was confirmed as non-workable. This app will not selected to boot at all. */
//			return "Invalid";
//		case ESP_OTA_IMG_ABORTED://         = 0x4U,         /*!< App could not confirm the workable or non-workable. In bootloader IMG_PENDING_VERIFY state will be changed to IMG_ABORTED. This app will not selected to boot at all. */
//			return "Aborted";
//		case ESP_OTA_IMG_UNDEFINED://       = 0xFFFFFFFFU,
//			return "Ota Img Undefined";
//	}
//	return "Undefined";
//}

