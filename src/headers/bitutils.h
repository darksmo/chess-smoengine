#ifndef BITUTILS_h
#define BITUTILS_h

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

#endif
