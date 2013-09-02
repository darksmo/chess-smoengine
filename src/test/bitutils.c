#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"

#include "bitutils.h"
#include "bitboard.h"


int tests_run = 0;

static char *test_cell_of_bit() {
    U64 n = 0x8000ULL;
    mu_assert("Correct cell of bit", 15 == _cell_of_bit(n));
    return 0;
}

static char *test_mask_between() {
    mu_assert("diagonal between cells 7 and 56", _mask_between(7, 56) == 0x2040810204000ULL);
    mu_assert("diagonal between cells 56 and 7", _mask_between(56, 7) == 0x2040810204000ULL);
    mu_assert("diagonal between cells 40 and 47", _mask_between(40, 47) == 0x7E0000000000ULL);
    mu_assert("diagonal between cells 41 and 33", _mask_between(41, 33) == 0x0ULL);
    mu_assert("diagonal between cells 33 and 41", _mask_between(33, 41) == 0x0ULL);
    mu_assert("diagonal between cells 26 and 2", _mask_between(26, 2) == 0x40400ULL);
    mu_assert("diagonal between cells 2 and 26", _mask_between(2, 26) == 0x40400ULL);
    return 0;
}


static char *all_tests() {
    mu_run_test(test_cell_of_bit);
    mu_run_test(test_mask_between);
    return 0;
}

int main(int argc, char **argv) 
{
    char *result = all_tests();
    if (result != 0) {
        printf("not ok - %s\n", result);
    }
    else {
        printf("\\o/ All bitutils tests passed!\n");
    }
    printf("Tests run: %d\n", tests_run);


    return result != 0;
}
