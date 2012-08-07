#include "engine.h"
#include "bitboard.h"

#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"


/* - - - - - - -  Tests for bitboards - - - - - - - - */
int tests_run = 0;

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

static char *test_64_bits_arithmetics() {
    U64 n;
    n = 0xFFFFFFFFFFFFFFFF;
    mu_assert("Can represent 64bit numbers", n == 0xFFFFFFFFFFFFFFFF);
    n = -1ULL;
    mu_assert("Got max number", n == 0xFFFFFFFFFFFFFFFF);
    n = 1ULL >> 10;
    mu_assert("Right shift works as expected", n == 0x0);
    n = 1ULL << 10;
    mu_assert("Left shift works as expected", n == 0x400);
    return 0;
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
	Bitboard *b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper);
    return b;
}

static char *test_bitboard_positions() {
    Bitboard *b = create_test_bitboard();
    mu_assert("White pawns correctly placed", b->position[WHITE_PAWN] == 0xFF00);
    mu_assert("Black rooks correctly placed", b->position[BLACK_ROOK] == 0x8100000000000000);
    mu_assert("Black queen correctly placed", b->position[BLACK_QUEEN] == 0x800000000000000);
    mu_assert("Black king correctly placed", b->position[BLACK_KING] == 0x1000000000000000);
    mu_assert("White queen correctly placed", b->position[WHITE_QUEEN] == 0x8);
    mu_assert("Black pawns correctly placed", b->position[BLACK_PAWN] == 0xFF000000000000);
    mu_assert("All white pieces in place", bitboard_get_white_positions(b) == 0xFFFF);
    mu_assert("All black pieces in place", bitboard_get_black_positions(b) == 0xFFFF000000000000);
    mu_assert("All pieces in place", bitboard_get_all_positions(b) == 0xFFFF00000000FFFF);
    mu_assert("Correct piece type in B2", get_piece_type(b, FILE_B, RANK_2) == WHITE_PAWN);
    mu_assert("Correct piece type in B2", get_piece_type(b, FILE_B, RANK_2) == WHITE_PAWN);
    destroy_bitboard(b);
    return 0;
}

static char *test_masks() {
    mu_assert("Masked rank 3", _mask_rank(RANK_3) == 0xFF0000);
    mu_assert("Masked file A", _mask_file(FILE_A) == 0x0101010101010101);
    mu_assert("Masked file C", _mask_file(FILE_C) == 0x0404040404040404ULL);
    mu_assert("Masked file H", _mask_file(FILE_H) == 0x8080808080808080ULL);
    mu_assert("Masked cell A1", _mask_cell(FILE_A, RANK_1) == 0x1ULL);
    mu_assert("Masked cell H1", _mask_cell(FILE_H, RANK_1) == 0x80ULL);
    mu_assert("Masked cell A2", _mask_cell(FILE_A, RANK_2) == 0x100ULL);
    mu_assert("Masked cell C3", _mask_cell(FILE_C, RANK_3) == 0x40000ULL);
    mu_assert("Masked cell H8", _mask_cell(FILE_H, RANK_8) == 0x8000000000000000ULL);
    return 0;
}

static char *test_clears() {
    mu_assert("Cleared rank 3", _clear_rank(RANK_3) == 0xFFFFFFFFFF00FFFFULL);
    mu_assert("Cleared rank 4", _clear_rank(RANK_4) == 0xFFFFFFFF00FFFFFFULL);
    mu_assert("Cleared file A", _clear_file(FILE_A) == 0xFEFEFEFEFEFEFEFEULL);
    mu_assert("Cleared file H", _clear_file(FILE_H) == 0x7F7F7F7F7F7F7F7FULL);
    return 0;
}

static char *test_legal() {
    Bitboard *b = create_test_bitboard();
    mu_assert("King cannot move", get_legal_moves(b, FILE_E, RANK_1) == 0ULL);
    destroy_bitboard(b);

	char *chessboard =
    /* bit 56 */  "k......k" 
    /* bit 48 */  "........"
    /* bit 40 */  "........"
    /* bit 32 */  "...K...."
    /* bit 24 */  ".......k" 
    /* bit 16 */  "........"
    /* bit  8 */  ".k......" 
    /* bit  0 */  "K......k";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper);
    mu_assert("King moves ok 1", get_legal_moves(b, FILE_D, RANK_5) == 0x1C141C000000ULL);
    // mu_assert("King moves ok 2", get_legal_moves(b, FILE_B, RANK_2) == 0x70404ULL);
    mu_assert("King moves ok 3", get_legal_moves(b, FILE_H, RANK_1) == 0xC040ULL);
    mu_assert("King moves ok 4", get_legal_moves(b, FILE_H, RANK_4) == 0xC040C00000);
    mu_assert("King moves ok 5", get_legal_moves(b, FILE_A, RANK_8) == 0x203000000000000ULL);
    mu_assert("King moves ok 6", get_legal_moves(b, FILE_H, RANK_8) == 0x40C0000000000000ULL);
    destroy_bitboard(b);
    
	chessboard =
    /* bit 56 */  ".n......" 
    /* bit 48 */  "........"
    /* bit 40 */  "...x...n"
    /* bit 32 */  "n.x...x."
    /* bit 24 */  ".n..n..." 
    /* bit 16 */  "..x...x."
    /* bit  8 */  "...x.x.." 
    /* bit  0 */  "........";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper);
    mu_assert("Knight moves ok 1", get_legal_moves(b, FILE_E, RANK_4) == 0x284400442800ULL);
    mu_assert("Knight moves ok 2", get_legal_moves(b, FILE_H, RANK_6) == 0x4020002040000000ULL);
    mu_assert("Knight moves ok 3", get_legal_moves(b, FILE_A, RANK_5) == 0x2040004020000ULL); 
    mu_assert("Knight moves ok 4", get_legal_moves(b, FILE_B, RANK_4) == 0x50800080500ULL); 
    mu_assert("Knight moves ok 5", get_legal_moves(b, FILE_B, RANK_8) == 0x8050000000000ULL);
    destroy_bitboard(b);
	
    return 0;
}

static char *all_tests() {
    mu_run_test(test_masks);
    mu_run_test(test_clears);
    mu_run_test(test_64_bits_arithmetics);
    mu_run_test(test_bitboard_positions);
    mu_run_test(test_legal);
    return 0;
}

int main(int argc, char **argv) 
{
    char *result = all_tests();
    if (result != 0) {
        printf("not ok - %s\n", result);
    }
    else {
        printf("\\o/ ALL TESTS PASSED!\n");
    }
    printf("Tests run: %d\n", tests_run);


    return result != 0;
}
