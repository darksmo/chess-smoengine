#include "engine.h"
#include "bitboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INFINITY 999999.9f

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

    float score = 
          (1.0f * score_material)
        + (0.1f * score_piece_count)
        + (0.5f * score_center_occupation)
    ;

    return score;
}

float evaluate_one_move(Bitboard *b, Move *m, PieceColor turn) {
    Bitboard *bb = clone_bitboard(b);
    bitboard_do_move(bb, m);

    float score = evaluate_bitboard(b, turn);

    destroy_bitboard(bb);

    return score;
}

float negaMax(Bitboard *b, Move *m, int depth, PieceColor turn) {
    if ( depth == 0 ) return evaluate_one_move(b, m, turn);
    float max = -INFINITY;

   /*
    * we need to understand what's the score after the current move
    * ... so let's generate b1 = move(b)
    */
    Bitboard *b1 = clone_bitboard(b);
    bitboard_do_move(b1, m);

    // invert turn and get ready to move all pieces of the opposite color
    PieceColor next_turn = PIECE_COLOR_BLACK;
    if (turn == PIECE_COLOR_BLACK) {
        next_turn = PIECE_COLOR_WHITE;
    }

    U64 piece_positions = (next_turn == PIECE_COLOR_WHITE) 
        ?  bitboard_get_white_positions(b1)
        :  bitboard_get_black_positions(b1);

    while (piece_positions) {
        Move next_move;
        init_move(&next_move);
        piece_positions = get_next_cell_in(piece_positions, &next_move);
        reset_legal_move_iterator(b1);
        while (get_next_legal_move(b1, &next_move)) {

            // score the move with negaMax, but invert the resulting score
            float score = -1 * negaMax(b1, &next_move, depth - 1, next_turn);

            if (score > max || max == -INFINITY) {
                max = score;
            }
        }
    }

    destroy_bitboard(b1);

    return max;
}

float get_best_move(Bitboard *b, Move *ptr_move_result, 
    PieceColor turn, void (*callback_best_move_found)(Move *))
{
    Move move;

    /* iterate through all moves of the current color */

    // get piece positions
    U64 piece_positions = (turn == PIECE_COLOR_WHITE) 
        ?  bitboard_get_white_positions(b)
        :  bitboard_get_black_positions(b);

    // the resulting maximum gain
    float max = -INFINITY;
    int n_legal_moves = 0;

    // we assume pieces are on the chessboard basically
    while (piece_positions) {
        init_move(&move);
        piece_positions = get_next_cell_in(piece_positions, &move);
        reset_legal_move_iterator(b);
        while (get_next_legal_move(b, &move)) {
            n_legal_moves++;

            // move contains the next legal move for white
            float score = negaMax(b, &move, 3, turn);

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
