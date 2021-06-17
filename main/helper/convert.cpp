#include "convert.hpp"
#include <ctype.h>
/**
 * Adapted from https://stackoverflow.com/a/1086059
 */
long long strntoll(const char *s, std::size_t n) noexcept
{
    long long x = 0;
    bool negative = false;

    if(n == 0) return 0;

    if(s[0] == '-')
    {
        s++; n--;
        negative = true;
    }
    else if(s[0] == '+')
    {
        s++; n--;
    }

    while(isdigit(s[0]) && n--)
    {
        x = x * 10 + (s[0] - '0');
        s++;
    }
    return x * (negative ? -1 : 1);
}
