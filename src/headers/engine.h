#ifndef ENGINE_h
#define ENGINE_h

#define ENGINE_MAX_MEMORY 2147483648
#define SCORE_INFINITE 99999999
#define MIN(x,y) ((x < y) ? x : y)

#include "bitboard.h"

typedef enum piece_color_t {
    PIECE_COLOR_WHITE,
    PIECE_COLOR_BLACK
} PieceColor;

/* turn: 0 = black, 1 = white */
float get_best_move(Bitboard *b, Move *ptr_move_result, 
    PieceColor turn, void (*callback_best_move_found)(Move *));

float evaluate_one_move(Bitboard *b, Move *m, PieceColor turn);

#endif
