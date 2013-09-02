#ifndef BITUTILS_h
#define BITUTILS_h

#define DIAGONAL(r,f) ((r-f) & 15)
#define ANTI_DIAGONAL(r,f) ((r+f) ^ 7)
#define _CELL(r,f) (r * 8 + f)
#define _RANK(c) (c >> 3)
#define _FILE(c) (c & 7)

/* 
 * Adapted from: 
 * https://blogs.oracle.com/DanX/entry/optimizing_byte_swapping_for_fun
 */
#define BSWAP_64(x) (((U64)(x) << 56) | \
        (((U64)(x) << 40) & 0xff000000000000ULL) | \
        (((U64)(x) << 24) & 0xff0000000000ULL) | \
        (((U64)(x) << 8)  & 0xff00000000ULL) | \
        (((U64)(x) >> 8)  & 0xff000000ULL) | \
        (((U64)(x) >> 24) & 0xff0000ULL) | \
        (((U64)(x) >> 40) & 0xff00ULL) | \
        ((U64)(x)  >> 56))

#define LS1B(x) ((x)&(-x))

typedef unsigned long long U64;

U64 _clear_file(unsigned int n);
U64 _clear_rank(unsigned int n);
U64 _mask_rank(unsigned int n);
U64 _mask_file(unsigned int n);
U64 _mask_cell(unsigned int file, unsigned int rank);
U64 _mirror(U64 bit);

U64 _mask_diag(unsigned int n);
U64 _mask_antidiag(unsigned int n);

U64 _mask_between(unsigned int n1, unsigned int n2);

/*
 * Returns the number of bits set to 1 in the give 64 bit integer.
 */
int _count_bits(U64 bit);

/*
 * Given a 64bit integer and a count, returns the position of the most
 * significant bit found.
 */
int _cell_of_bit(U64 bit);

#endif
