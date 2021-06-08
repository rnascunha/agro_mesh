#include "my_string.h"
#include <stdio.h>

My_String::My_String(size_t size){
	str = (char*)malloc(sizeof(char)*size);
	len = 0;
	this->size = size;
}

My_String::My_String(const char* str){
	size = len = strlen(str);
	this->str = (char*)malloc(len*sizeof(char));
	memcpy(this->str, str, len);
}

My_String::My_String(const char* str, size_t len){
	size = this->len = len;
	this->str = (char*)malloc(len*sizeof(char));
	memcpy(this->str, str, len);
}

My_String::~My_String(){
	free(str);
}

int My_String::substring(char* buffer, const char* delimiter, size_t pos /* = 0 */){
	if(pos >= len){
		return -1;
	}

	size_t len = this->len - pos;
	char *str = this->str + pos;

	size_t i;
	for(i = 0; i < len; i++){
		size_t s = 0;
		while(delimiter[s]){
			if(*str == delimiter[s++]){
				*buffer = '\0';
				return i + pos;
			}
		}
		*buffer = *str;
		buffer++; str++;
	}
	*buffer = '\0';
	return i + pos;
}

int My_String::substring_console(char* buffer, const char* delimiter, size_t pos /* = 0 */){
	if(pos >= len){
		return -1;
	}

	size_t len = this->len - pos;
	char *str = this->str + pos;

	size_t i;
	for(i = 0; i < len; i++){
		size_t s = 0;
		if(*str == '\'' || *str == '"'){
			char quot = *str;
			do{
				if(i++ >= len){
					return -1;
				}
				*buffer = *str;
				buffer++; str++;
			}while(*str != quot);
		}

		while(delimiter[s]){
			if(*str == delimiter[s++]){
				*buffer = '\0';
				return i + pos;
			}
		}
		*buffer = *str;
		buffer++; str++;
	}
	*buffer = '\0';
	return i + pos;
}
