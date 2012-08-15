#ifndef BITBOARD_h
#define BITBOARD_h

#include "bitutils.h"

#define DIAGONAL(r,f) ((r-f) & 15)
#define ANTI_DIAGONAL(r,f) ((r+f) ^ 7)
#define _CELL(r,f) (r * 8 + f)
#define _RANK(c) (c >> 3)
#define _FILE(c) (c & 7)
#define _CELL_WHITE_KING_HOME 4
#define _CELL_BLACK_KING_HOME 60
#define _CELL_WHITE_KING_LEFTCASTLE 2
#define _CELL_WHITE_KING_RIGHTCASTLE 6
#define _CELL_BLACK_KING_LEFTCASTLE 58
#define _CELL_BLACK_KING_RIGHTCASTLE 62

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
    PIECE_TYPE_COUNT, /* 12 */
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
    /* where a given type of piece is */
	U64 position[PIECE_TYPE_COUNT]; 

    U64 white_remaining_pawns_longsteps;
    U64 black_remaining_pawns_longsteps;
    U64 white_castling_rights;
    U64 black_castling_rights;

    /* used for both black and white, since only a bit at a time can be set. */
    U64 enpassant_rights; 

    /* which type of piece is at a given cell */
    PieceType piece_type[64]; 

    /* used to compute the next legal move */
    U64 legal_move_iterator;
    U64 legal_move_iterator_lastcell;
} Bitboard;

Bitboard *create_bitboard(void *chessboard_base, unsigned int chessboard_element_size, PieceType (*func_type_mapper)(void *), int reverse_ranks);
Bitboard *create_blank_bitboard();
Bitboard *clone_bitboard();
Bitboard *create_blank_bitboard();
void destroy_bitboard(Bitboard *bitboard);

/* I may cache these for efficiency */
int is_legal_move(Bitboard *b, Move *m);
void bitboard_do_move(Bitboard *b, Move *m);
U64 bitboard_get_white_positions(Bitboard *b);
U64 bitboard_get_black_positions(Bitboard *b);
U64 bitboard_get_all_positions(Bitboard *b);

char *bitboard_piece_name(PieceType t);
void print_bitboard(Bitboard *b);
void print_chessboard(Bitboard *b);
void print_bits(U64 b);
U64 get_legal_moves(Bitboard *b, FileType file, RankType rank);
int get_next_legal_move(Bitboard *b, Move *ptr_move_dest);

PieceType get_piece_type(Bitboard *b, FileType file, RankType rank);

#endif
