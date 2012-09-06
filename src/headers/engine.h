#ifndef ENGINE_h
#define ENGINE_h

#define ENGINE_MAX_MOVES_TO_SCORE 15
#define SCORE_INFINITE 99999999

#include "bitboard.h"

typedef enum piece_color_t {
    PIECE_COLOR_WHITE,
    PIECE_COLOR_BLACK
} PieceColor;

int get_best_move(Bitboard *b, Move *ptr_move_result, PieceColor turn);
double evaluate_one_move(Bitboard *b, Move *m);

#endif
