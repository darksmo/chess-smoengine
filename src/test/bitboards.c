#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"

#include "bitboard.h"


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
	Bitboard *b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    print_chessboard(b);
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
    Move m;
    m.promote_to = PIECE_NONE;
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
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    mu_assert("King moves ok 1", get_legal_moves(b, FILE_D, RANK_5) == 0x1C141C000000ULL);
    // mu_assert("King moves ok 2", get_legal_moves(b, FILE_B, RANK_2) == 0x70404ULL);
    mu_assert("King moves ok 3", get_legal_moves(b, FILE_H, RANK_1) == 0xC040ULL);
    mu_assert("King moves ok 4", get_legal_moves(b, FILE_H, RANK_4) == 0xC040C00000);
    mu_assert("King moves ok 5", get_legal_moves(b, FILE_A, RANK_8) == 0x203000000000000ULL);
    mu_assert("King moves ok 6", get_legal_moves(b, FILE_H, RANK_8) == 0x40C0000000000000ULL);
    destroy_bitboard(b);

	
    /* king looses castling rights */
    chessboard =
    /* bit 56 */  "r...k..r" 
    /* bit 48 */  "........"
    /* bit 40 */  "........"
    /* bit 32 */  "........"
    /* bit 24 */  "........" 
    /* bit 16 */  "........"
    /* bit  8 */  "........" 
    /* bit  0 */  "........";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    mu_assert("King may casle correctly 1", get_legal_moves(b, FILE_E, RANK_8) == 0x6c38000000000000LLU);
    m.from_file = FILE_A; 
    m.from_rank = RANK_8; 
    m.to_file = FILE_A;
    m.to_rank = RANK_1;
    bitboard_do_move(b, &m);
    mu_assert("King still castles right", get_legal_moves(b, FILE_E, RANK_8) == 0x6838000000000000LLU);
    m.from_file = FILE_H; 
    m.from_rank = RANK_8; 
    m.to_file = FILE_H;
    m.to_rank = RANK_7;
    bitboard_do_move(b, &m);
    mu_assert("King cannot castle", get_legal_moves(b, FILE_E, RANK_8) == 0x2838000000000000LLU);
    destroy_bitboard(b);
    
    /* king castles correctly-1*/
    chessboard =
    /* bit 56 */  "r...k..r" 
    /* bit 48 */  "........"
    /* bit 40 */  "........"
    /* bit 32 */  "........"
    /* bit 24 */  "........" 
    /* bit 16 */  "........"
    /* bit  8 */  "........" 
    /* bit  0 */  "........";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    m.from_file = FILE_E;
    m.from_rank = RANK_8;
    m.to_file = FILE_C;
    m.to_rank = RANK_8;
    bitboard_do_move(b, &m);
    print_chessboard(b);
    mu_assert("Found rook in the expected place", BLACK_ROOK == get_piece_type(b, FILE_D, RANK_8));
    mu_assert("Found king in the expected place", BLACK_KING == get_piece_type(b, FILE_C, RANK_8));
    
    /* king castles correctly-2*/
    chessboard =
    /* bit 56 */  "r...k..r" 
    /* bit 48 */  "........"
    /* bit 40 */  "........"
    /* bit 32 */  "........"
    /* bit 24 */  "........" 
    /* bit 16 */  "........"
    /* bit  8 */  "........" 
    /* bit  0 */  "........";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    m.from_file = FILE_E;
    m.from_rank = RANK_8;
    m.to_file = FILE_G;
    m.to_rank = RANK_8;
    bitboard_do_move(b, &m);
    print_chessboard(b);
    mu_assert("Found rook in the expected place", BLACK_ROOK == get_piece_type(b, FILE_F, RANK_8));
    mu_assert("Found king in the expected place", BLACK_KING == get_piece_type(b, FILE_G, RANK_8));
    /* move to top-right corner */
    m.from_file = FILE_G;
    m.from_rank = RANK_8;
    m.to_file = FILE_H;
    m.to_rank = RANK_8;
    bitboard_do_move(b, &m);
    mu_assert("Found king in the expected place", BLACK_KING == get_piece_type(b, FILE_H, RANK_8));
    /* move back */
    m.from_file = FILE_H;
    m.from_rank = RANK_8;
    m.to_file = FILE_G;
    m.to_rank = RANK_8;
    bitboard_do_move(b, &m);
    mu_assert("Found king in the expected place", BLACK_KING == get_piece_type(b, FILE_G, RANK_8));

	chessboard =
    /* bit 56 */  ".n......" 
    /* bit 48 */  "........"
    /* bit 40 */  "...x...n"
    /* bit 32 */  "n.x...x."
    /* bit 24 */  ".n..n..." 
    /* bit 16 */  "..x...x."
    /* bit  8 */  "...x.x.." 
    /* bit  0 */  "........";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    mu_assert("Knight moves ok 1", get_legal_moves(b, FILE_E, RANK_4) == 0x284400442800ULL);
    mu_assert("Knight moves ok 2", get_legal_moves(b, FILE_H, RANK_6) == 0x4020002040000000ULL);
    mu_assert("Knight moves ok 3", get_legal_moves(b, FILE_A, RANK_5) == 0x2040004020000ULL); 
    mu_assert("Knight moves ok 4", get_legal_moves(b, FILE_B, RANK_4) == 0x50800080500ULL); 
    mu_assert("Knight moves ok 5", get_legal_moves(b, FILE_B, RANK_8) == 0x8050000000000ULL);
    destroy_bitboard(b);
	
    chessboard =
    /* bit 56 */  ".R......" 
    /* bit 48 */  ".r..Q..P"
    /* bit 40 */  ".......p"
    /* bit 32 */  "r....R.."
    /* bit 24 */  "........" 
    /* bit 16 */  "...R...."
    /* bit  8 */  "..k....r" 
    /* bit  0 */  "r.......";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    mu_assert("Pieces occupancy is right for rook test", bitboard_get_all_positions(b) == 0x292802100088401ULL);
    mu_assert("Rook moves ok 1", get_legal_moves(b, FILE_A, RANK_1) == 0x10101feULL);
    mu_assert("Rook moves ok 2", get_legal_moves(b, FILE_D, RANK_3) == 0x808080808f70808ULL);
    mu_assert("Rook moves ok 3", get_legal_moves(b, FILE_F, RANK_5) == 0x202020df20202020ULL);
    mu_assert("Rook moves ok 4", get_legal_moves(b, FILE_H, RANK_2) == 0x8080807880ULL);
    mu_assert("Rook moves ok 5", get_legal_moves(b, FILE_B, RANK_7) == 0x21d020202020202ULL);
    mu_assert("Rook moves ok 6", get_legal_moves(b, FILE_A, RANK_5) == 0x101013e01010100ULL);
    destroy_bitboard(b);
    
    chessboard =
    /* bit 56 */  "........" 
    /* bit 48 */  "..b....."
    /* bit 40 */  "........"
    /* bit 32 */  "....P.P."
    /* bit 24 */  "........" 
    /* bit 16 */  "........"
    /* bit  8 */  "...B...." 
    /* bit  0 */  ".......b";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    mu_assert("Bishop moves ok 1", get_legal_moves(b, FILE_C, RANK_7) == 0xa000a1100000000ULL);
    mu_assert("Bishop moves ok 2", get_legal_moves(b, FILE_D, RANK_2) == 0x122140014ULL);
    mu_assert("Bishop moves ok 3", get_legal_moves(b, FILE_H, RANK_1) == 0x102040810204000ULL);
    destroy_bitboard(b);

    chessboard =
    "r.bqkb.r"
    "pp.n..pp"
    "..n.pp.."
    "...pP..."
    "...P.P.."
    ".....N.."
    "PP....PP"
    "R.BQKBNR";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    mu_assert("Bishop moves ok 4", get_legal_moves(b, FILE_F, RANK_1) == 0x10204081000LLU);
    destroy_bitboard(b);

    chessboard =
    /* bit 56 */  "........" 
    /* bit 48 */  "..q....."
    /* bit 40 */  "........"
    /* bit 32 */  "....P.P."
    /* bit 24 */  "........" 
    /* bit 16 */  "........"
    /* bit  8 */  "...Q...." 
    /* bit  0 */  ".......q";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    mu_assert("Queen moves ok 1", get_legal_moves(b, FILE_C, RANK_7) == 0xefb0e1504040404ULL);
    mu_assert("Queen moves ok 2", get_legal_moves(b, FILE_D, RANK_2) == 0x80808092a1cf71cULL);
    mu_assert("Queen moves ok 3", get_legal_moves(b, FILE_H, RANK_1) == 0x8182848890a0c07fULL);
    destroy_bitboard(b);
    
    chessboard =
    /* bit 56 */  "........" 
    /* bit 48 */  ".p......"
    /* bit 40 */  "....P..."
    /* bit 32 */  ".......p"
    /* bit 24 */  "......PP" 
    /* bit 16 */  "...p...."
    /* bit  8 */  "....P..." 
    /* bit  0 */  "........";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    mu_assert("Pawn moves ok 1", get_legal_moves(b, FILE_E, RANK_2) == 0x10180000LLU);
    mu_assert("Pawn moves ok 2", get_legal_moves(b, FILE_E, RANK_6) == 0x10000000000000LLU);
    mu_assert("Pawn moves ok 3", get_legal_moves(b, FILE_B, RANK_7) == 0x20200000000LLU);
    mu_assert("Pawn moves ok 4", get_legal_moves(b, FILE_H, RANK_5) == 0x40000000LLU);

    /* Queen aiming at a queen of the opposite color */
    chessboard =
    /* bit 56 */  "...r.rk."
    /* bit 48 */  ".p.....p"
    /* bit 40 */  "p.p.bpp."
    /* bit 32 */  "....qP.."
    /* bit 24 */  "....P..."
    /* bit 16 */  ".P..N..."
    /* bit  8 */  "PQ.R..PP"
    /* bit  0 */  "....R.K.";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    mu_assert("Black queen moves ok", get_legal_moves(b, FILE_E, RANK_5) == 0x204082f38448200LLU);

    /* en-passant */
    chessboard =
    /* bit 56 */  "........"
    /* bit 48 */  "p...p..."
    /* bit 40 */  "........"
    /* bit 32 */  "........"
    /* bit 24 */  "........"
    /* bit 16 */  "........"
    /* bit  8 */  "...P...."
    /* bit  0 */  "........";
	b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper, 0);
    m.from_file = FILE_D;  /* d2-d4 */
    m.from_rank = RANK_2;
    m.to_file = FILE_D;
    m.to_rank = RANK_4;
    bitboard_do_move(b, &m);
    
    m.from_file = FILE_A; /* a7-a6 */
    m.from_rank = RANK_7;
    m.to_file = FILE_A;
    m.to_rank = RANK_6;
    bitboard_do_move(b, &m);
    
    m.from_file = FILE_D; /* d4-d5 */
    m.from_rank = RANK_4;
    m.to_file = FILE_D;
    m.to_rank = RANK_5;
    bitboard_do_move(b, &m);
    print_chessboard(b);
    
    m.from_file = FILE_E; /* e7-e5 */
    m.from_rank = RANK_7;
    m.to_file = FILE_E;
    m.to_rank = RANK_5;
    bitboard_do_move(b, &m);
    
    m.from_file = FILE_D; /* d5-e6 */
    m.from_rank = RANK_5;
    m.to_file = FILE_E;
    m.to_rank = RANK_6;
    mu_assert("En-passant capture works as expected", is_legal_move(b, &m));
    bitboard_do_move(b, &m);

    print_bitboard(b);
    print_chessboard(b);

    destroy_bitboard(b);


    unsigned int ii=0;
    for (ii=0; ii<=15; ii++) {
        print_bits(_mask_antidiag(ii));
    }
	
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
