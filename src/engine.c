#include "engine.h"
#include "bitboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INFINITY 999999.9f
#define DEPTH 4
#define NBITS_IN_INT sizeof(int) * 8

// populate scores
float _piece_score [] = {
    // must reflect PieceType
	100, // WHITE_PAWN, 
	300, // WHITE_KNIGHT,
	325, // WHITE_BISHOP,
	500, // WHITE_ROOK,
	900, // WHITE_QUEEN,
	99999, // WHITE_KING,
	100, // BLACK_PAWN,
    300, // BLACK_KNIGHT,
	325, // BLACK_BISHOP,
	500, // BLACK_ROOK,
	900, // BLACK_QUEEN,
	99999, // BLACK_KING,
    0, // PIECE_TYPE_COUNT,
    0 // PIECE_NONE  
};

float get_score_material_difference (Bitboard *b) {
    // white material
    float score_material_white = 0.0f;
    U64 white_positions = bitboard_get_white_positions(b);
    Move position;
    PieceType piece_type;
    while (white_positions) {
        white_positions = get_next_cell_in(white_positions, &position);
        piece_type = get_piece_type(b, position.from_file, position.from_rank);
        score_material_white += _piece_score[piece_type];
    }
    
    // black material
    float score_material_black = 0.0f;
    U64 black_positions = bitboard_get_black_positions(b);
    while (black_positions) {
        black_positions = get_next_cell_in(black_positions, &position);
        piece_type = get_piece_type(b, position.from_file, position.from_rank);
        score_material_black += _piece_score[piece_type];
    }

    return score_material_white - score_material_black;
}

float evaluate_bitboard(Bitboard *b, PieceColor turn) {
    float white_or_black = 1.0f; // white
    if (turn == PIECE_COLOR_BLACK) {
        white_or_black = -1.0f; // black
    }

    float score_material = get_score_material_difference(b) * white_or_black;

    float score_piece_count = 
        (float)(bitboard_get_white_count(b) - bitboard_get_black_count(b)) * white_or_black;
    
    float score_center_occupation = 
        (float)(bitboard_get_white_center_count(b) - bitboard_get_black_center_count(b)) * white_or_black;

    U64 center_attackers = bitboard_get_center_attackers(b);
    int n_white_attackers = center_attackers & ~bitboard_get_black_positions(b);
    int n_black_attackers = center_attackers & ~bitboard_get_white_positions(b);

    float score_center_attackers =
        (float)(n_white_attackers - n_black_attackers) * white_or_black;

    float score = 
          (0.9f * score_material)
        + (0.1f * score_piece_count)
        + (0.2f * score_center_occupation)
        + (0.6f * score_center_attackers)
    ;

    return score;
}

float evaluate_one_move(Bitboard *b, Move *m, PieceColor turn) {
    Bitboard *bb = clone_bitboard(b);
    bitboard_do_move(bb, m);

    float score = evaluate_bitboard(bb, turn);

    destroy_bitboard(bb);

    return score;
}

float negaMax(Bitboard *b, Move *m, int depth, PieceColor turn, float alpha, float beta, Move move_history[]) {
    if ( depth == 0 ) { 
        return evaluate_one_move(b, m, turn);
    }


    float max = -INFINITY;

   /*
    * we need to understand what's the score after the current move
    * ... so let's generate b1 = move(b)
    */
    Bitboard *b1 = clone_bitboard(b);
    bitboard_do_move(b1, m);

    // invert turn and get ready to move all pieces of the opposite color
    PieceColor next_turn = (turn == PIECE_COLOR_BLACK) 
        ? PIECE_COLOR_WHITE
        : PIECE_COLOR_BLACK;

    U64 piece_positions = (next_turn == PIECE_COLOR_WHITE) 
        ?  bitboard_get_white_positions(b1)
        :  bitboard_get_black_positions(b1);

    Move next_move;
    init_move(&next_move);
    while (piece_positions) {
        piece_positions = get_next_cell_in(piece_positions, &next_move);
        reset_legal_move_iterator(b1);
        while (get_next_legal_move(b1, &next_move)) {

            // score the move with negaMax, but invert the resulting score
            float score = -1 * negaMax(b1, &next_move, depth - 1, next_turn, -beta, -alpha, move_history);

            if (score > alpha) {
                alpha = score;
            }

            if (beta <= alpha) {
                destroy_bitboard(b1);
                return alpha;
            }
        }
    }


    destroy_bitboard(b1);
    return alpha;
}

float get_best_move(Bitboard *b, Move *ptr_move_result, 
    PieceColor turn, void (*callback_best_move_found)(Move *))
{
    int should_assign_max;
    Move move;

    /* iterate through all moves of the current color */

    // get piece positions
    U64 piece_positions = (turn == PIECE_COLOR_WHITE) 
        ?  bitboard_get_white_positions(b)
        :  bitboard_get_black_positions(b);

    // the resulting maximum gain
    float max = -INFINITY;
    int n_legal_moves = 0;

    Move move_history[DEPTH];

    // we assume pieces are on the chessboard basically
    while (piece_positions) {
        init_move(&move);
        piece_positions = get_next_cell_in(piece_positions, &move);
        reset_legal_move_iterator(b);
        while (get_next_legal_move(b, &move)) {
            n_legal_moves++;

            // move contains the next legal move for white
            float score = negaMax(b, &move, DEPTH - 1, turn, -INFINITY-1, INFINITY+1, move_history);

            // if score is equal we decide randomly whether to assign best move
            if (max == score) {
                // check if odd/even
                should_assign_max = (
                    ((unsigned int) rand() << NBITS_IN_INT - 1
                ) >> NBITS_IN_INT - 1) ;

                // trigger max assignment
                if (should_assign_max) {
                    max = score - 1;
                }
            }

            // keep the best next legal move according to negamax
            if (max < score) {
                max = score;
                memcpy(ptr_move_result, &move, sizeof(Move));

                if (callback_best_move_found != NULL) {
                    callback_best_move_found(&move);
                }
            }
        }
    }

    // declare checkmate
    if (!n_legal_moves) {
        PieceType king_piece = (turn == PIECE_COLOR_WHITE) ?
            WHITE_KING :
            BLACK_KING;
        U64 king_square = b->position[king_piece];
        int cell = _cell_of_bit(king_square);
        ptr_move_result->from_rank = _RANK(cell);
        ptr_move_result->to_rank = _RANK(cell);
        ptr_move_result->from_file = _FILE(cell);
        ptr_move_result->to_file = _FILE(cell);
        ptr_move_result->is_checkmate = 1;
        return -INFINITY;
    }

    return max;
}
