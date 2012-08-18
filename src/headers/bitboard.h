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

#define MASK_WHITE_KING_RIGHT_CASTLE 0x40ULL
#define MASK_WHITE_KING_LEFT_CASTLE 0x4ULL
#define MASK_BLACK_KING_RIGHT_CASTLE 0x4000000000000000ULL
#define MASK_BLACK_KING_LEFT_CASTLE 0x400000000000000ULL

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
	WHITE_PAWN, 
	WHITE_KNIGHT,
	WHITE_BISHOP,
	WHITE_ROOK,
	WHITE_QUEEN,
	WHITE_KING,
	BLACK_PAWN,
    BLACK_KNIGHT,
	BLACK_BISHOP,
	BLACK_ROOK,
	BLACK_QUEEN,
	BLACK_KING,
    PIECE_TYPE_COUNT,
    PIECE_NONE  
} PieceType;

typedef struct {
    FileType from_file;
    RankType from_rank;
    FileType to_file;
    RankType to_rank;
    PieceType promote_to;
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
    void *pieces_addr[64];

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
void print_chessboard_move(Bitboard *b, Move *m);
void print_bits(U64 b);
U64 get_legal_moves(Bitboard *b, FileType file, RankType rank);
int get_next_legal_move(Bitboard *b, Move *ptr_move_dest);

PieceType get_piece_type(Bitboard *b, FileType file, RankType rank);
void *get_piece_addr(Bitboard *b, FileType file, RankType rank);

#endif
