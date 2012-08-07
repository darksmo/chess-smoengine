#include "bitboard.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


Bitboard *create_bitboard(void *chessboard_base, unsigned int chessboard_element_size, PieceType (*func_type_mapper)(void *))
{
    Bitboard *b = malloc(sizeof(Bitboard));
    char *c = (char*) chessboard_base;
    bzero(b, sizeof(Bitboard));

    /* create the initial bitboard */
    int r,i,cell;
    cell = 0;
    for (r=0; r<64; r+=8) {
        for (i=r+7; i >= r; i--) {
            PieceType t = (*func_type_mapper)(chessboard_base + chessboard_element_size * i);

            if (t != PIECE_NONE)
                b->position[t] = b->position[t] | 1ULL << 63 - cell;
                b->piece_type[63 - cell] = t;

            cell++;
        }
    }
    
    /* return it */
    return b;
}

void destroy_bitboard(Bitboard *bitboard) 
{
	free(bitboard);
}

void print_bitboard(Bitboard *b)
{
    char *piece_type_name[] = {
        "WHITE PAWN",
        "WHITE KNIGHT",
        "WHITE BISHOP",
        "WHITE ROOK",
        "WHITE QUEEN",
        "WHITE KING",
        "BLACK PAWN",
        "BLACK KNIGHT",
        "BLACK BISHOP",
        "BLACK ROOK",
        "BLACK QUEEN",
        "BLACK KING"
    };

    int i;
    printf(" - - - position bitboards - - - \n");
    for (i=0; i<PIECE_COUNT; i++) {
        printf("%s) %llu\n", piece_type_name[i], b->position[i]);
    }
}

void print_bits(U64 bits)
{
    U64 bits_w = bits;
    int nbits = 64;
    while(nbits) {
        nbits-=8;
        printf("%c ", 0x100000000000000ULL  & bits_w ? '1' : '-');
        printf("%c ", 0x200000000000000ULL  & bits_w ? '1' : '-');
        printf("%c ", 0x400000000000000ULL  & bits_w ? '1' : '-');
        printf("%c ", 0x800000000000000ULL  & bits_w ? '1' : '-');
        printf("%c ", 0x1000000000000000ULL & bits_w ? '1' : '-');
        printf("%c ", 0x2000000000000000ULL & bits_w ? '1' : '-');
        printf("%c ", 0x4000000000000000ULL & bits_w ? '1' : '-');
        printf("%c ", 0x8000000000000000ULL & bits_w ? '1' : '-');
        bits_w <<= 8;
        printf ("\n");
    }
    printf("\n");
}

U64 bitboard_get_white_positions(Bitboard *b)
{
    U64 positions = 0ULL;
    int i;
    for (i=WHITE_PAWN; i<=WHITE_KING; i++)
        positions |= b->position[i];
    return positions;
}

U64 bitboard_get_black_positions(Bitboard *b)
{
    U64 positions = 0ULL;
    int i;
    for (i=BLACK_PAWN; i<=BLACK_KING; i++)
        positions |= b->position[i];
    return positions;
}

U64 bitboard_get_all_positions(Bitboard *b)
{
    return (bitboard_get_black_positions(b) | bitboard_get_white_positions(b));
}


U64 get_piece_type(Bitboard *b, FileType file, RankType rank)
{
    return b->piece_type[rank * 8 + file];
}

U64 get_legal_moves(Bitboard *b, FileType file, RankType rank) 
{
    PieceType t = get_piece_type(b, file, rank);
    U64 piece_pos = b->position[t] & _mask_cell(file, rank);
    U64 result = 0ULL;            

    /* king/knight */
    U64 pclip_a = piece_pos & _clear_file(FILE_A);
    U64 pclip_h = piece_pos & _clear_file(FILE_H);
    U64 pclip_b = piece_pos & _clear_file(FILE_B);
    U64 pclip_g = piece_pos & _clear_file(FILE_G);

    /* rooks */
    U64 occupancy = bitboard_get_all_positions(b);
    U64 rook_file_mask = _mask_file(file) & ~piece_pos;
    U64 rook_forward, rook_reverse;

    switch (t) {
        case WHITE_KING:
        case BLACK_KING:
            /* 
             * TODO: the king cannot move on squares checked by the pieces
             * of the oppsite color! 
             */
            result = (pclip_a >> 1) 
               | (pclip_a >> 9)
               | (pclip_a << 7)
               | (pclip_h << 1)
               | (pclip_h << 9)
               | (pclip_h >> 7)
               | (piece_pos << 8)
               | (piece_pos >> 8);
            break;
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            result = 
                 (pclip_g & pclip_h) >> 6
               | (pclip_b & pclip_a) >> 10
               | (pclip_h) >> 15
               | (pclip_a) >> 17
               | (pclip_b & pclip_a) << 6
               | (pclip_g & pclip_h) << 10
               | (pclip_a) << 15
               | (piece_pos) << 17;
            break;
        case WHITE_ROOK:
        case BLACK_ROOK:
            /* horizontal attacks */
            result = _mask_rank(rank)
                & ( (occupancy - (2 * piece_pos)) 
                    ^ _mirror( _mirror(occupancy) - (2 * _mirror(piece_pos))) 
                );

            /* vertical attacks */
            rook_forward = occupancy & rook_file_mask;
            rook_reverse = BSWAP_64(rook_forward);
            rook_forward -= (piece_pos); 
            rook_reverse -= BSWAP_64(piece_pos);
            rook_forward ^= BSWAP_64(rook_reverse);
            result |= rook_forward & rook_file_mask;

            //result &= (rook_file_mask | _mask_rank(rank));
            break;
    }
    
    if (piece_pos & bitboard_get_white_positions(b))
        result &= ~bitboard_get_white_positions(b);
    else
        result &= ~bitboard_get_black_positions(b);

    printf("Piece type: %d FILE: %d RANK: %d : 0x%llxULL\n", t, file, rank, result);
    print_bits(result);
    return result;
}
