#ifndef BITBOARD_h
#define BITBOARD_h

#include "bitutils.h"

typedef enum file_type_t {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
} FileType;

typedef enum rank_type_t {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
} RankType;

typedef enum piece_type_t {
	WHITE_PAWN,   /* 0 */
	WHITE_KNIGHT, /* 1 */
	WHITE_BISHOP, /* 2 */
	WHITE_ROOK,   /* 3 */
	WHITE_QUEEN,  /* 4 */
	WHITE_KING,   /* 5 */
	BLACK_PAWN,   /* 6 */
    BLACK_KNIGHT, /* 7 */
	BLACK_BISHOP, /* 8 */
	BLACK_ROOK,   /* 9 */
	BLACK_QUEEN,  /* 10 */
	BLACK_KING,   /* 11 */
    PIECE_COUNT,  /* 12 */
    PIECE_NONE    /* 13 */
} PieceType;

typedef struct {
    FileType from_file;
    RankType from_rank;
    FileType to_file;
    RankType to_rank;
    const char* as_string;
} Move;

typedef struct {
	U64 position[PIECE_COUNT]; /* where a given type of piece is */
    PieceType piece_type[64];  /* which type of piece is at a given cell */
} Bitboard;

Bitboard *create_bitboard(void *chessboard_base, unsigned int chessboard_element_size, PieceType (*func_type_mapper)(void *));

/* I may cache these for efficiency */
U64 bitboard_get_white_positions(Bitboard *b);
U64 bitboard_get_black_positions(Bitboard *b);
U64 bitboard_get_all_positions(Bitboard *b);

void destroy_bitboard(Bitboard *bitboard);
void print_bitboard(Bitboard *b);
void print_bits(U64 b);
U64 get_legal_moves(Bitboard *b, FileType file, RankType rank);

#endif
