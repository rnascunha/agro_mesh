#include <stdio.h>
#include <stdint.h>

uint32_t local_main(uint32_t arg)
{
    printf("Hello world!\n");
    printf("It works args[%u]!!\n", arg);
    return 2;
}
