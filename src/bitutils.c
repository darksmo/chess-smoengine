#include "bitutils.h"

const U64 k1 = (0x5555555555555555); /*  -1/3   */
const U64 k2 = (0x3333333333333333); /*  -1/5   */
const U64 k4 = (0x0f0f0f0f0f0f0f0f); /*  -1/17  */
const U64 kf = (0x0101010101010101); /*  -1/255 */

U64 _antidiag_masks[] = {
 /* 0 */   0x102040810204080ULL,
 /* 1 */   0x1020408102040ULL,
 /* 2 */   0x10204081020ULL,
 /* 3 */   0x102040810ULL,
 /* 4 */   0x1020408ULL,
 /* 5 */   0x10204ULL,
 /* 6 */   0x102ULL, 
 /* 7 */   0x1ULL, 
 /* 8 */   0x0ULL,
 /* 9 */   0x8000000000000000ULL,
 /* 10 */  0x4080000000000000ULL,
 /* 11 */  0x2040800000000000ULL,
 /* 12 */  0x1020408000000000ULL,
 /* 13 */  0x810204080000000ULL,
 /* 14 */  0x408102040800000ULL,
 /* 15 */  0x204081020408000ULL
};

U64 _diag_masks[] = {
 /* 0 */   0x8040201008040201ULL,
 /* 1 */   0x4020100804020100ULL, 
 /* 2 */   0x2010080402010000ULL,
 /* 3 */   0x1008040201000000ULL,
 /* 4 */   0x804020100000000ULL,
 /* 5 */   0x402010000000000ULL,
 /* 6 */   0x201000000000000ULL,
 /* 7 */   0x100000000000000ULL,
 /* 8 */   0x0ULL,
 /* 9 */   0x80ULL, 
 /* 10 */  0x8040ULL, 
 /* 11 */  0x804020ULL,
 /* 12 */  0x80402010ULL,
 /* 13 */  0x8040201008ULL, 
 /* 14 */  0x804020100804ULL,
 /* 15 */  0x80402010080402ULL
};

U64 _mask_antidiag(unsigned int n)
{
    return _antidiag_masks[n];
}

U64 _mask_diag(unsigned int n)
{
    return _diag_masks[n];
}

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

int _cell_of_bit(U64 bit) {
    int cell_of_next_move = 0;
    while(bit >>= 1) {
        cell_of_next_move++;
    }
    return cell_of_next_move;
}

U64 _cache_mask_between[64][64];
int _was_cache_mask_between_initialized = 0;
U64 _mask_between(unsigned int n1, unsigned int n2) {
    if (!_was_cache_mask_between_initialized) {
        // populate cache
        int i, k;
        U64 result;
        int dx, dy;
        for (i=0; i<64; i++) {
            for (k=0; k<64; k++) {
                int x1 = _FILE(i);
                int y1 = _RANK(i);
                int x2 = _FILE(k);
                int y2 = _RANK(k);
                result = 0ULL;
                while ((x1 != x2) || (y1 != y2)) {
                    dx = x2 - x1;
                    dy = y2 - y1;
                    if (dx) {
                        x1 += (dx > 0) ? 1 : -1;
                    }
                    if (dy) {
                        y1 += (dy > 0) ? 1 : -1;
                    }
                    result |= _mask_cell(x1, y1);
                }
                result &= ~_mask_cell(x2, y2);
                _cache_mask_between[i][k] = result;
            }
        }
        _was_cache_mask_between_initialized = 1;
    }
    return _cache_mask_between[n1][n2];
}

int _count_bits(U64 bit) {
    bit =  bit       - ((bit >> 1)  & k1); /* put count of each 2 bits into those 2 bits */
    bit = (bit & k2) + ((bit >> 2)  & k2); /* put count of each 4 bits into those 4 bits */
    bit = (bit       +  (bit >> 4)) & k4 ; /* put count of each 8 bits into those 8 bits */
    bit = (bit * kf) >> 56; /* returns 8 most significant bits of bit + (bit<<8) + (bit<<16) + (bit<<24) + ...  */
    return (int) bit;
}
