#ifndef CONVERT_CHAR_TYPE_H__
#define CONVERT_CHAR_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

char* convert_utf8_to_ascii(char* str);
char utf8_to_ascii(const uint8_t utf8_char);

#ifdef __cplusplus
}
#endif

#endif /* CONVERT_CHAR_TYPE_H__ */
