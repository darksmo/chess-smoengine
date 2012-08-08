#include <stdio.h>
#include <stdlib.h>

#include "../bitboard.h"

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
	Bitboard *b = create_bitboard((void *)chessboard, sizeof(char), &type_mapper);
    return b;
}

/*
 * Try to parse chess games in hyphenated algbraic notation, and see if we
 * agree for moves in there to be legal.
 */
int main(int argc, char **argv) {

    /* will fill up the move as we parse it from each line */
    Move move;

    /* start from a default chessboard */
	Bitboard *chessboard = create_default_chessboard();
    
    const char *filename = "../games/Adams.whalg";
    FILE *file = fopen(filename, "r");
    if (file) {
       char line [128];
       while (fgets(line, sizeof line, file)) {
          
          /* skip unwanted lines */
          if (line[0] != '[' && line[0] != ' ') {
            
              /* parse wanted line */
              printf("Parsing: %s\n", line);
          }
       }
       fclose (file);
    }
    else {
       perror(filename);
    }
}
