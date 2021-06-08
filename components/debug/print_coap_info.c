#include "print_coap_info.h"
#include "get_type_string.h"
#include <stdio.h>
#include <math.h>

void print_uri(coap_uri_t* uri){
	printf("Host: %.*s\n",uri->host.length, (char*)uri->host.s);
	printf("Path: %.*s\n",uri->path.length, (char*)uri->path.s);
	printf("Query: %.*s\n",uri->query.length, (char*)uri->query.s);
	printf("Port: %u\n",uri->port);
	printf("Scheme: %s\n", get_coap_uri_scheme_string(uri->scheme));

	#define BUFSIZE 40
    unsigned char _buf[BUFSIZE];
    unsigned char *buf;
    size_t buflen;
    int res;

	if (uri->path.length) {
		buflen = BUFSIZE;
		buf = _buf;
		res = coap_split_path(uri->path.s, uri->path.length, buf, &buflen);

		printf("Path[%d]: ", res);
		while (res--) {
			printf("%.*s ", coap_opt_length(buf), (char*)coap_opt_value(buf));
			buf += coap_opt_size(buf);
		}
	}

	if (uri->query.length) {
		buflen = BUFSIZE;
		buf = _buf;
		res = coap_split_query(uri->query.s, uri->query.length, buf, &buflen);

		printf("\nQuery[%d]: ", res);
		while (res--) {
			printf("%.*s ", coap_opt_length(buf), (char*)coap_opt_value(buf));
			buf += coap_opt_size(buf);
		}
	}
	printf("\n");
}

void print_uri_string(const char* uri_name){
	coap_uri_t uri;
	printf("\nUri: %s\n", uri_name);
	if (coap_split_uri((const uint8_t *)uri_name, strlen(uri_name), &uri) == -1) {
		printf("CoAP server uri error");
		return;
	}

	print_uri(&uri);
}

void print_coap_pdu(coap_pdu_t* pdu, bool request){
	printf("Type: %s\n", get_coap_type_message_string(pdu->type));
	printf("Token len: %u\n", pdu->token_length);
	if(request){
		printf("Method: %s\n", get_coap_method_string(pdu->code));
	} else {
		printf("Code: %s\n", get_coap_code_string(pdu->code));
	}
//	printf("", pdu->);
	printf("Message ID: %u\n", pdu->tid);
	if(pdu->token_length){
		printf("Token: %.*s\n", pdu->token_length, (char*)pdu->token);
	}

	printf("Options:\n");
	coap_opt_iterator_t opt_iter;
	coap_opt_t *opt;
	coap_option_iterator_init(pdu, &opt_iter, COAP_OPT_ALL);

	while ((opt = coap_option_next(&opt_iter))){
		printf("D:%03u|L:%03u|%s: ",
				coap_opt_delta(opt),
				coap_opt_length(opt),
				get_coap_option_string(opt_iter.type));
		switch(opt_iter.type){
			case COAP_OPTION_IF_MATCH:			//   1 /* C, opaque, 0-8 B, (none) */
				break;
			case COAP_OPTION_URI_HOST:			//   3 /* C, String, 1-255 B, destination address */
				printf("%.*s", coap_opt_length(opt), (char*)coap_opt_value(opt));
				break;
			case COAP_OPTION_ETAG:				//   4 /* E, opaque, 1-8 B, (none) */
			case COAP_OPTION_IF_NONE_MATCH:		//   5 /* empty, 0 B, (none) */
				break;
			case COAP_OPTION_URI_PORT:			//   7 /* C, uint, 0-2 B, destination port */
				printf("%u", coap_decode_var_bytes(coap_opt_value(opt), coap_opt_length(opt)));
				break;
			case COAP_OPTION_LOCATION_PATH:		//   8 /* E, String, 0-255 B, - */
				printf("%.*s", coap_opt_length(opt), (char*)coap_opt_value(opt));
				break;
			case COAP_OPTION_URI_PATH:			//   11 /* C, String, 0-255 B, (none) */
				printf("%.*s", coap_opt_length(opt), (char*)coap_opt_value(opt));
				break;
			case COAP_OPTION_CONTENT_FORMAT:	//   12 /* E, uint, 0-2 B, (none) */
//			case COAP_OPTION_CONTENT_TYPE:		//   COAP_OPTION_CONTENT_FORMAT
				printf("%s", get_coap_media_type_string(*coap_opt_value(opt)));
				break;
			case COAP_OPTION_MAXAGE:			//   14 /* E, uint, 0--4 B, 60 Seconds */
				printf("%u", coap_decode_var_bytes(coap_opt_value(opt), coap_opt_length(opt)));
				break;
			case COAP_OPTION_URI_QUERY:			//   15 /* C, String, 1-255 B, (none) */
				printf("%.*s", coap_opt_length(opt), (char*)coap_opt_value(opt));
				break;
			case COAP_OPTION_ACCEPT:			//   17 /* C, uint, 0-2 B, (none) */
				printf("%s", get_coap_media_type_string(*coap_opt_value(opt)));
				break;
			case COAP_OPTION_LOCATION_QUERY:	//   20 /* E, String, 0-255 B, (none) */
				printf("%.*s", coap_opt_length(opt), (char*)coap_opt_value(opt));
				break;
			case COAP_OPTION_SIZE2:				//   28 /* E, uint, 0-4 B, (none) */
				printf("%u", coap_decode_var_bytes(coap_opt_value(opt), coap_opt_length(opt)));
				break;
			case COAP_OPTION_PROXY_URI:			//   35 /* C, String, 1-1034 B, (none) */
				printf("%.*s", coap_opt_length(opt), (char*)coap_opt_value(opt));
				break;
			case COAP_OPTION_PROXY_SCHEME:		//   39 /* C, String, 1-255 B, (none) */
				printf("%.*s", coap_opt_length(opt), (char*)coap_opt_value(opt));
				break;
			case COAP_OPTION_SIZE1:				//   60 /* E, uint, 0-4 B, (none) */
				printf("%u", coap_decode_var_bytes(coap_opt_value(opt), coap_opt_length(opt)));
				break;
			case COAP_OPTION_OBSERVE:			//   6 /* E, empty/uint, 0 B/0-3 B, (none) */
//			case COAP_OPTION_SUBSCRIPTION:		//   COAP_OPTION_OBSERVE
			case COAP_OPTION_BLOCK2:			//   23 /* C, uint, 0--3 B, (none) */
				printf("%u|%u|%u", coap_opt_block_num(opt),
						COAP_OPT_BLOCK_MORE(opt) >> 3,
						1 << (COAP_OPT_BLOCK_SZX(opt) + 4));
				break;
			case COAP_OPTION_BLOCK1:			//   27 /* C, uint, 0--3 B, (none) */
				printf("%u|%u|%u", coap_opt_block_num(opt),
										COAP_OPT_BLOCK_MORE(opt) >> 3,
										1 << (COAP_OPT_BLOCK_SZX(opt) + 4));
				break;
			case COAP_OPTION_NORESPONSE:		//   258 /* N, uint, 0--1 B, 0 */
				break;
		}
		printf("\n");
	}

//	coap_string_t* s = coap_get_uri_path(pdu);
//	printf("Full path: %.*s\n", s->length, (char*)s->s);

	size_t data_len;
	uint8_t *data;
	coap_get_data(pdu, &data_len, &data);
	if(data_len){
		printf("Data[%u]: %.*s\n", data_len, data_len, data);
	} else {
		printf("Data[0]: no data\n");
	}
}

static char* fixed_point_to_string(char* buffer, size_t buffer_len, coap_fixed_point_t* p){
	snprintf(buffer, buffer_len, "%u.%03u", p->integer_part, p->fractional_part);

	return buffer;
}

static float fixed_point_to_float(coap_fixed_point_t* p){
	return (float)((float)p->integer_part + (((float)p->fractional_part) / 1000));
}

static float calculate_max_transmission_span(coap_session_t* session){
	int max_restransmit;
	coap_fixed_point_t ack_timeout, ack_random_factor;

	if(!session){
		ack_timeout = COAP_DEFAULT_ACK_TIMEOUT;
		max_restransmit = COAP_DEFAULT_MAX_RETRANSMIT;
		ack_random_factor = COAP_DEFAULT_ACK_RANDOM_FACTOR;
	}else {
		ack_timeout = coap_session_get_ack_timeout(session);
		max_restransmit = coap_session_get_max_transmit(session);
		ack_random_factor = coap_session_get_ack_random_factor(session);
	}

	return (float)(fixed_point_to_float(&ack_timeout) *
			((pow(2, max_restransmit)) - 1) *
			fixed_point_to_float(&ack_random_factor));
}

static float calculate_max_transmission_wait(coap_session_t* session){
	int max_restransmit;
	coap_fixed_point_t ack_timeout, ack_random_factor;

	if(!session){
		ack_timeout = COAP_DEFAULT_ACK_TIMEOUT;
		max_restransmit = COAP_DEFAULT_MAX_RETRANSMIT;
		ack_random_factor = COAP_DEFAULT_ACK_RANDOM_FACTOR;
	}else {
		ack_timeout = coap_session_get_ack_timeout(session);
		max_restransmit = coap_session_get_max_transmit(session);
		ack_random_factor = coap_session_get_ack_random_factor(session);
	}

	return (float)(fixed_point_to_float(&ack_timeout) *
			(pow(2, (max_restransmit + 1)) - 1) *
			fixed_point_to_float(&ack_random_factor));
}

void print_coap_transmission_param(coap_session_t* session){
	int max_restransmit;
	coap_fixed_point_t ack_timeout, ack_random_factor;
	char buffer[40];

	if(!session){
		ack_timeout = COAP_DEFAULT_ACK_TIMEOUT;
		max_restransmit = COAP_DEFAULT_MAX_RETRANSMIT;
		ack_random_factor = COAP_DEFAULT_ACK_RANDOM_FACTOR;
	}else {
		ack_timeout = coap_session_get_ack_timeout(session);
		max_restransmit = coap_session_get_max_transmit(session);
		ack_random_factor = coap_session_get_ack_random_factor(session);
	}

	printf("Ack Timeout: %s\n", fixed_point_to_string(buffer, 40, &ack_timeout));
	printf("Ack Random Factor: %s\n", fixed_point_to_string(buffer, 40, &ack_random_factor));
	printf("Max restransmission: %d\n", max_restransmit);

	if(session){
		printf("Max pdu size: %u\n", coap_session_max_pdu_size(session));
	}

	printf("Max Transmission Span: %f\n", calculate_max_transmission_span(session));
	printf("Max Transmission Wait: %f\n", calculate_max_transmission_wait(session));
}
