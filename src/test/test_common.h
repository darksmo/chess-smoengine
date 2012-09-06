/*
 * To be included exactly once in tests that need to create a default
 * chessboard.
 */
#include "bitboard.h"

PieceType type_mapper(void *el)
{
    char * x = (char*) el;
    switch (*x) {
        case 'r': return BLACK_ROOK;
        case 'n': return BLACK_KNIGHT;
        case 'b': return BLACK_BISHOP;
        case 'q': return BLACK_QUEEN;
        case 'k': return BLACK_KING;
        case 'p': return BLACK_PAWN;
        case 'R': return WHITE_ROOK;
        case 'N': return WHITE_KNIGHT;
        case 'B': return WHITE_BISHOP;
        case 'Q': return WHITE_QUEEN;
        case 'K': return WHITE_KING;
        case 'P': return WHITE_PAWN;
    }
    return PIECE_NONE;
}

static Bitboard *create_test_bitboard() {
	char *chessboard =
        "rnbqkbnr"
        "pppppppp"
        "........"
        "........"
        "........"
        "........"
        "PPPPPPPP"
        "RNBQKBNR";
	Bitboard *b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    print_chessboard(b);
    return b;
}
