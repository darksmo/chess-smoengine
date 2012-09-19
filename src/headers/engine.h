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

typedef struct {
    Bitboard * b;
    double score;
    PieceColor turn;
    unsigned long int id_parent;
    unsigned long int id_minchild;
    unsigned long int id_maxchild;
} EvalNode;

int get_best_move(Bitboard *b, Move *ptr_move_result, PieceColor turn);
double evaluate_one_move(Bitboard *b, Move *m);

#endif
