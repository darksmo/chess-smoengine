#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"

#include "bitutils.h"


int tests_run = 0;

static char *test_cell_of_bit() {
    U64 n = 0x8000ULL;
    mu_assert("Correct cell of bit", 15 == _cell_of_bit(n));
    return 0;
}


static char *all_tests() {
    mu_run_test(test_cell_of_bit);
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
