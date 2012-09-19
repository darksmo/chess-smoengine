#include "engine.h"
#include "bitboard.h"

#include <stdio.h>
#include <stdlib.h>

double evaluate_one_move(Bitboard *b, Move *m) {
    return (double) (rand() % 100);
}

/* turn: 0 = black, 1 = white */

int get_best_move(Bitboard *b, Move *ptr_move_result, PieceColor turn) {
    
    /* in case we reach the maximum limit */
    int flag_nomem = 0;

    /* scored bitboard tree */
    unsigned long int max_moves_to_score = ENGINE_MAX_MEMORY / (sizeof(Bitboard) + sizeof(EvalNode));
    EvalNode *tree = malloc(sizeof(EvalNode) * max_moves_to_score);
    printf("Attempting to score %ld moves\n", max_moves_to_score);
    if (!tree) {
        fprintf(stderr, "Error allocating memory for tree\n");
        exit(1);
    }

    /* 
     * Populate initial roots 
     */
    unsigned long int next = 0;
    unsigned long int first = 0;
    unsigned long int last = 0;

    /* iterate through all moves of the current color */
    U64 piece_positions = (turn == PIECE_COLOR_WHITE) 
        ?  bitboard_get_white_positions(b)
        :  bitboard_get_black_positions(b);

    Move move;
    while (piece_positions && !flag_nomem) {

        /* evaluate all legal moves */
        init_move(&move);
        piece_positions = get_next_cell_in(piece_positions, &move);
        while (get_next_legal_move(b, &move) && !flag_nomem) {

            /* make the move on a clone of the bitboard */
            Bitboard *bb = clone_bitboard(b);
            
            /* make move (we now have the to* fields filled) */
            bitboard_do_move(bb, &move);

            /* save result of move evaluation */
            /* tree[next].score = evaluate_one_move(tree[next].b, &ptr_move); */
            tree[next].b = bb;
            tree[next].turn = turn;
            
            next++;
            if (next >= max_moves_to_score) {
                flag_nomem = 1;
            }
        }
    }
    last = MIN(next - 1, max_moves_to_score);
        
    /* flip turn */
    turn = (PIECE_COLOR_WHITE == turn) ? PIECE_COLOR_BLACK : PIECE_COLOR_WHITE;

    /* --- now score the roots iteratively --- */
    while (first <= last) {
    
        unsigned long int c;
        for (c=first; c<=last; c++) {

            if (flag_nomem) {
                tree[c].id_minchild = 0;
                tree[c].id_maxchild = 0;
            }
            else {
                tree[c].id_minchild = next;
                piece_positions = (turn == PIECE_COLOR_WHITE) 
                    ?  bitboard_get_white_positions(tree[c].b)
                    :  bitboard_get_black_positions(tree[c].b);

                /* evaluate all legal moves */
                while (piece_positions && !flag_nomem) {
                    init_move(&move);
                    piece_positions = get_next_cell_in(piece_positions, &move);

                    Bitboard *parent_b = tree[c].b; 
                    Bitboard *child_b;
                
                    reset_legal_move_iterator(parent_b);
                    while (get_next_legal_move(parent_b, &move) && !flag_nomem) {
                        child_b = clone_bitboard(parent_b);
                        bitboard_do_move(child_b, &move);
                        tree[next].id_parent = c;
                        tree[next].b = child_b;
                        tree[next].turn = turn;
                        next++;
                        
                        if (next >= max_moves_to_score) {
                            flag_nomem = 1;
                        }
                    }
                }
                tree[c].id_maxchild = next - 1;
            }
        }

        /* should we process the next set of parents? */
        if (flag_nomem) {
            first = last + 1;
        }
        else {
            first = last + 1;
            last = next - 1;
            printf("Processed chunk %ld - %ld\n", first, last);

            /* flip turn */
            turn = (PIECE_COLOR_WHITE == turn) ? PIECE_COLOR_BLACK : PIECE_COLOR_WHITE;
        }
    }

    printf("Processed %ld total chunks\n", next);

    // unsigned long int dd;
    // for (dd=0; dd<next; dd++) {
    //         char *turn = (tree[dd].turn == PIECE_COLOR_BLACK ) ? "black" : "white";
    //         printf("Chess %ld  Turn: %s  Parent id: %ld\n", dd, turn, tree[dd].id_parent);
    //         print_chessboard(tree[dd].b);
    // }

    return 1;
}
