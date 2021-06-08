#ifndef PRINT_TYPES_INFO_H__
#define PRINT_TYPES_INFO_H__

#include <stdio.h>
#include <time.h>

void print_basic_types_size(){
	printf("char: %u |uchar: %u\n"
			"int: %u |uint: %u\n"
			"lint: %u |luint: %u\n"
			"llint: %u |lluint: %u\n",
				sizeof(char), sizeof(unsigned char),
				sizeof(int), sizeof(unsigned int),
				sizeof(long int), sizeof(long unsigned int),
				sizeof(long long int), sizeof(long long unsigned int));

	printf("int8: %u |uint8: %u\n"
			"int16: %u |uint16: %u\n"
			"int32: %u |uint32: %u\n"
			"int64: %u |uint64: %u\n",
				sizeof(int8_t), sizeof(uint8_t),
				sizeof(int16_t), sizeof(uint16_t),
				sizeof(int32_t), sizeof(uint32_t),
				sizeof(int64_t), sizeof(uint64_t));

	printf("float: %u | double: %u\n",
			sizeof(float), sizeof(double));

	printf("time_t: %u\n", sizeof(time_t));

}

#endif /* PRINT_TYPES_INFO_H__ */
