#include "decompose.hpp"

static bool parse_ipv4(in_addr& host, char* uri_string, char** new_uri_string) noexcept
{
	char addr[16];
	int i = 0;
	while(uri_string[0] != '\0' && i != 16)
	{
		if(!CoAP::Helper::is_digit(uri_string[0]) && uri_string[0] != '.') break;
		addr[i] = uri_string[0];
		uri_string += 1;
		i++;
		addr[i] = '\0';
	}
	*new_uri_string = uri_string;

	if(i == 0 || i == 16) return false;
	if(uri_string[0] == '/'
		|| uri_string[0] == '?'
		|| uri_string[0] == ':'
		|| uri_string[0] == '#'
		|| uri_string[0] == '\0') {
		int ret = inet_pton(AF_INET, addr, &host);
		if(ret <= 0) return false;
		return true;
	}
	return false;
}

bool decompose_ipv4(CoAP::URI::uri<in_addr>& uri, char* uri_string) noexcept
{
	uri.path_len = uri.query_len = 0;
	char* nuri_string = uri_string;

	/**
	 * Host
	 */
	if(!parse_ipv4(uri.host, uri_string, &nuri_string)) return false;
	uri_string = nuri_string;
//	return true;
	/**
	 * Separator
	 */
	if(uri_string[0] == '\0') return true;
	if(uri_string[0] ==  ':')
	{
		uri_string += 1;
		/**
		 * Port
		 */
		if(!decompose_port(uri, uri_string, &nuri_string)) return false;
		uri_string = nuri_string;
	}

	return true;
}
