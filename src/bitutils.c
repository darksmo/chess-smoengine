#include "bitutils.h"

U64 _mask_rank(unsigned int n)
{
    return 0xFFULL << 0x8 * n;
}
U64 _clear_file(unsigned int n)
{
    return ~_mask_file(n);
}
U64 _clear_rank(unsigned int n)
{
   return ~_mask_rank(n);
}
U64 _mask_file(unsigned int n)
{
   return 0x101010101010101ULL << n;
}
U64 _mask_cell(unsigned int file, unsigned int rank)
{
    return _mask_file(file) & _mask_rank(rank);
}

U64 _mirror(U64 bit)
{
    int i;
    for (i=0; i<4; i++) {
        U64 amask = 0x101010101010101ULL << i;  // initially the a file mask
        U64 bmask = 0x8080808080808080ULL >> i; // initially the h file mask
        U64 namask = ~amask;
        U64 nbmask = ~bmask;

        U64 a = bit & amask;
        U64 b = bit & bmask;
        bit &= namask;
        bit &= nbmask;

        bit = bit | (a << (7-(2*i)));
        bit = bit | (b >> (7-(2*i)));
    }
    return bit;
}
