#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"

#include "test_common.h"
#include "engine.h"


int tests_run = 0;

static char *test_get_best_move() {
    Move m_result;
    Bitboard *b = create_test_bitboard();
    mu_assert("Obtained the best move", 1 == get_best_move(b, &m_result, PIECE_COLOR_WHITE));

    printf("The best move:\n");
    // print_chessboard_move(b, &m_result);
   
    // destroy_bitboard(b);
    return 0;
}


static char *all_tests() {
    mu_run_test(test_get_best_move);
    return 0;
}

int main(int argc, char **argv) 
{
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
