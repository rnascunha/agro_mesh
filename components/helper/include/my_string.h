#ifndef MY_STRING_H__
#define MY_STRING_H__

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
class My_String{
	public:
		My_String(size_t size);
		My_String(const char* str);
		My_String(const char* str, size_t len);
		~My_String();

		int substring(char* buffer, const char* delimiter, size_t pos = 0);
		int substring_console(char* buffer, const char* delimiter, size_t pos = 0);

		bool operator==(const My_String& a){
			return (this->len == a.len) && strncmp(this->str, a.str, this->len) == 0;
		}

	protected:
		char* str;
		size_t len;
		size_t size;
};


#endif /* MY_STRING_H__ */
