#include "engine.h"
#include "bitboard.h"

#include <stdio.h>
#include <stdlib.h>

double evaluate_one_move(Bitboard *b, Move *m) {
    return (double) (rand() % 100);
}

/* turn: 0 = black, 1 = white */

int get_best_move(Bitboard *b, Move *ptr_move_result, PieceColor turn) {
    Bitboard *bclone;

    /* best moves and scores */
    Move * moves = malloc(sizeof(Move) * ENGINE_MAX_MOVES_TO_SCORE);
    if (!moves) {
        fprintf(stderr, "fatal: cannot allocate memory for moves\n");
        exit(1);
    }
    double scores[ENGINE_MAX_MOVES_TO_SCORE];

    /* an index over moves and scores */
    int ms_idx = 0;
    int ms_idx_old = 0;

    /* the maximum score found so far */
    int max_move_idx = 0;
    scores[0] = -1 * SCORE_INFINITE;

    /* iterate through all moves of the current color */
    U64 piece_positions = (turn == PIECE_COLOR_WHITE) 
        ?  bitboard_get_white_positions(b)
        :  bitboard_get_black_positions(b);

    int max_hit = 0;
                                                       
    Move * ptr_move = &(moves[ms_idx]);

    while (piece_positions) {

        /* fills from* fields */
        piece_positions = get_next_cell_in(piece_positions, ptr_move);
        
        /* evaluate all legal moves */
        while (get_next_legal_move(b, ptr_move)) {

            /* make the move on a clone of the bitboard */
            bclone = clone_bitboard(b);
        
            /* make move (we now have the to* fields filled) */
            print_chessboard_move(bclone, ptr_move);
            bitboard_do_move(bclone, ptr_move);

            /* save result of move evaluation */
            scores[ms_idx] = evaluate_one_move(b, ptr_move);
            evaluate_one_move(bclone, ptr_move);
            if (scores[ms_idx] > scores[max_move_idx]) {
                max_move_idx = ms_idx;
            }

            /* 
             * Now need to prepare for the next move from the same cell, hence:
             *
             * 1) save the old position that contains the from* fields 
             * 2) increment to new position
             * 3) copy old from* fields to the next move to try
             */
            ms_idx++;
            if (ms_idx >= ENGINE_MAX_MOVES_TO_SCORE) {
                ms_idx = 0;
                max_hit = 1;
            }
            moves[ms_idx].from_rank = ptr_move->from_rank;
            moves[ms_idx].from_file = ptr_move->from_file;
            ptr_move = &(moves[ms_idx]);

            /* restore bitboard for next move */
            destroy_bitboard(bclone);
        }
    }

    /* store the best move */
    ptr_move_result->from_file = moves[max_move_idx].from_file;
    ptr_move_result->to_file = moves[max_move_idx].to_file;
    ptr_move_result->from_rank = moves[max_move_idx].from_rank;
    ptr_move_result->to_rank = moves[max_move_idx].to_rank;

    free(moves);

    return 1;
}
