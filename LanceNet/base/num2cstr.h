#ifndef LanceNet_NUM2ASCII_H
#define LanceNet_NUM2ASCII_H
#include <algorithm>

// itoa
// Mattew Wilson 'Efficient Integer to String Conversions'
// MT-Safe
// the invoker have the responsible to allocated enough buf memory

namespace LanceNet{

template<typename IntegerType>
const char* itoa_s(char * dest, IntegerType integer)
{
    bool isNegative = integer < 0 ? true : false;
    static const char dict[] = "9876543210123456789";
    char* p = dest;
    int base = 9;

    while(integer){
        int r = integer % 10;
        *p++ = dict[base + r];
        integer /= 10;
    }

    if(isNegative)
    {
        *p++ = '-';
    }

    std::reverse(dest, p);
    *p++ = '\0';
    return dest;
}

} // namespace LanceNet

#endif // LanceNet_NUM2ASCII_H
