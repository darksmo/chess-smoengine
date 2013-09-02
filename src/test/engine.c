#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"

#include "test_common.h"
#include "engine.h"


int tests_run = 0;

static char *test_get_best_move() {
    Move m_result;
    Bitboard *b = create_test_bitboard();
    float score = get_best_move(b, &m_result, PIECE_COLOR_WHITE, NULL);
    mu_assert("got score >= 0", score >= 0.0f);

    return 0;
}


static char *all_tests() {
    mu_run_test(test_get_best_move);
    return 0;
}

int main(int argc, char **argv) 
{
    Move m_result;
    Bitboard *b = create_test_bitboard();
    PieceColor color = PIECE_COLOR_BLACK;

    char *result = all_tests();
    if (result != 0) {
        printf("not ok - %s\n", result);
    }
    else {
        printf("\\o/ All engine tests passed!\n");
    }
    printf("Tests run: %d\n", tests_run);


    return result != 0;
}
