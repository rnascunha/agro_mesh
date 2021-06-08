#include "convert_char_type.h"
#include <string.h>

char* convert_utf8_to_ascii(char* str) {
	unsigned int k = 0;
	unsigned int length = strlen(str);

	for (unsigned int i = 0; i < length; i++) {
		char c = utf8_to_ascii(str[i]);
		if (c != 0)	str[k++]=c;
	}

	str[k] = '\0';

	// This will leak 's' be sure to free it in the calling function.
	return str;
}

char utf8_to_ascii(const uint8_t utf8_char){
    // UTF-8 to font table index converter
    // Code form http://playground.arduino.cc/Main/Utf8ascii
	static uint8_t LASTCHAR;

	if (utf8_char < 128) { // Standard ASCII-set 0..0x7F handling
		LASTCHAR = 0;
		return utf8_char;
	}

	uint8_t last = LASTCHAR;   // get last char
	LASTCHAR = utf8_char;

	switch (last) {    // conversion depnding on first UTF8-character
		case 0xC2: return (uint8_t) utf8_char;
		case 0xC3: return (uint8_t) (utf8_char | 0xC0);
		case 0x82: if (utf8_char == 0xAC) return (uint8_t) 0x80;    // special case Euro-symbol
	}

	return (uint8_t) 0; // otherwise: return zero, if character has to be ignored
}
