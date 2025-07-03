
#include <utils/chars.h>

bool is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_digit(char c)
{
    return (c >= '0' && c <= '9');
}
