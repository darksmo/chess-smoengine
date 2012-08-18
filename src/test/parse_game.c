#include <stdio.h>
#include <stdlib.h>

#include "bitboard.h"

/* - - - - - SOME DECLARATIONS - - - - - - */

int verbose = 0;

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

Bitboard *create_default_chessboard() {
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
    return b;
}

typedef enum {
    READ_MOVE_NUMBER,
    READ_FIRST_FILE,
    READ_FIRST_RANK,
    READ_SECOND_FILE,
    READ_SECOND_RANK,
    READ_THIRD_FILE,
    READ_THIRD_RANK,
    READ_FOURTH_FILE,
    READ_FOURTH_RANK,
    STATE_COUNT 
} ParserLookFor;

/* - - - - - - GLOBAL PARSER STATE  - - - - - - */ 
Bitboard *chessboard = 0;
ParserLookFor state = READ_MOVE_NUMBER;

/* - - - - - - EVENTS CALLED FROM PARSER - - - - - - */ 

void new_game() 
{
    if (chessboard) {
        destroy_bitboard(chessboard);
    }
    chessboard = create_default_chessboard();
    if (verbose) { 
        printf("Created Chessboard\n");
        print_chessboard(chessboard);
    }
}

int move_counter = 0;
int do_move(Move *m)
{
    move_counter++;
    if (verbose) {
        printf("[%d] Move %c%c - %c%c ", 
           move_counter,
           m->from_file + 97, m->from_rank + 49,
           m->to_file + 97, m->to_rank + 49
        );
    }
    if (is_legal_move(chessboard, m)) {
        if (verbose) { printf("OK\n"); }
        bitboard_do_move(chessboard, m);
        if (verbose) { print_chessboard_move(chessboard, m); }
        return 1;
    }
    printf("!!! NOT LEGAL !!!\n");
    print_chessboard_move(chessboard, m);
    printf("The current chessboard looks like:\n");
    print_chessboard(chessboard);
    print_bitboard(chessboard);
    return 0;
}

void end_game()
{
    printf("--- end of game --- \n");
    destroy_bitboard(chessboard);
    chessboard = NULL;

    printf("Validated %d total moves.\n", move_counter);
}



/* - - - - - PARSER & HELPERS - - - - - - - */

int is_valid_char(char ch)
{
    switch (state){
        case READ_MOVE_NUMBER:
            if (ch == '-') { 
                /* HALT CONDITION */
                end_game(); 
                new_game();
                state = READ_MOVE_NUMBER; 
                return 0; 
            }
            switch (ch) {
                case '.': return 1;
                default : return 0;
            }
            break;
        case READ_FIRST_FILE:
        case READ_THIRD_FILE:
            if (ch == '-') { 
                /* HALT CONDITION */
                end_game(); 
                new_game();
                state = READ_MOVE_NUMBER; 
                return 0; 
            }
        case READ_FOURTH_FILE:
        case READ_SECOND_FILE:
            if (ch >= 0x61 && ch <= 0x68) return 1; /* a - h */
            return 0;

        case READ_FIRST_RANK:
        case READ_SECOND_RANK:
        case READ_THIRD_RANK:
        case READ_FOURTH_RANK:
            if (ch >= 0x31 && ch <= 0x38) return 1;
            return 0;
    }

    return 0;
}

PieceType maybe_promote_piece(char ch, ParserLookFor state) {
    if (state == READ_SECOND_RANK) {
        switch (ch) {
            case 'Q': return WHITE_QUEEN;
            case 'R': return WHITE_ROOK;
            case 'N': return WHITE_KNIGHT;
            case 'B': return WHITE_BISHOP;
        }
    }
    else {
        switch (ch) {
            case 'Q': return BLACK_QUEEN;
            case 'R': return BLACK_ROOK;
            case 'N': return BLACK_KNIGHT;
            case 'B': return BLACK_BISHOP;
        }
    }
    return PIECE_NONE;
}

int main(int argc, char **argv) {
    /* will fill up the move as we parse it */
    Move move = {0};

    /* start from a default chessboard */
    Bitboard *chessboard = create_default_chessboard();

    /* the game to load */
    const char *filename = "../games/all_games.whalg";

    /* expect the first move for a new game */
    new_game();

    FILE *file = fopen(filename, "r");
    if (file) {
        /* bunch of variables used by the parser */
        char line [128];
        char ch;
        int all_legal_moves_so_far = 1;
        unsigned int line_idx;
        ParserLookFor old_state;


        while (all_legal_moves_so_far 
               && fgets(line, sizeof(line), file)) {

            /* skip unwanted lines */
            if (line[0] != '[' && line[0] != ' ') {

                /* inspect a line */
                printf("Parsing: %s", line);
                line_idx = 0;

                while (all_legal_moves_so_far 
                       && (ch = line[line_idx++])) {

                    old_state = state;
                    if (is_valid_char(ch)) {
                        if (old_state != state) continue;
                        switch (state) {
                            case READ_MOVE_NUMBER:
                                break;
                            case READ_FIRST_FILE: 
                            case READ_THIRD_FILE:
                                move.from_file = ch - 97;
                                break;
                            case READ_FIRST_RANK: 
                            case READ_THIRD_RANK:
                                move.from_rank = ch - 49;
                                break;
                            case READ_SECOND_FILE:
                            case READ_FOURTH_FILE:
                                move.to_file = ch - 97;
                                break;
                            case READ_SECOND_RANK:
                            case READ_FOURTH_RANK:
                                move.to_rank = ch - 49;
                                
                                /* before making the move, check if we are going to promote */
                                move.promote_to = maybe_promote_piece(line[line_idx], state);

                                all_legal_moves_so_far = do_move(&move);
                                break;
                        }
                        state++;
                        state = state >= STATE_COUNT ? READ_MOVE_NUMBER : state;
                    }
                }
            }
        }
        fclose (file);
        end_game();
    }
    else {
        perror(filename);
    }

    return 0;
}
