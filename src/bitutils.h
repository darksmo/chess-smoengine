#ifndef BITUTILS_h
#define BITUTILS_h

typedef unsigned long long U64;

U64 _clear_file(unsigned int n);
U64 _clear_rank(unsigned int n);
U64 _mask_rank(unsigned int n);
U64 _mask_file(unsigned int n);
U64 _mask_cell(unsigned int file, unsigned int rank);

#endif
