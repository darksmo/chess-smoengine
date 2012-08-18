#include "bitboard.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Bitboard *create_blank_bitboard()
{
    Bitboard *b = malloc(sizeof(Bitboard));
    bzero(b, sizeof(Bitboard));
    return b;
}

Bitboard *clone_bitboard(Bitboard *b)
{
    Bitboard * new_b = create_blank_bitboard();
    memcpy(new_b, b, sizeof(Bitboard));
    return new_b; 
}

Bitboard *create_bitboard(void *chessboard_base, unsigned int chessboard_element_size, PieceType (*func_type_mapper)(void *), int reverse_ranks)
{
    Bitboard *b = create_blank_bitboard();
    
    /* All back and white pawns can move by two in the beginning */
    b->white_remaining_pawns_longsteps = _mask_rank(RANK_2);
    b->black_remaining_pawns_longsteps = _mask_rank(RANK_7);
    b->white_castling_rights = 0x0ULL;
    b->black_castling_rights = 0x0ULL;
    b->enpassant_rights = 0x0ULL;
    b->legal_move_iterator = 0x0ULL;

    /* 
     * this is supposed to be a single bit identifying the last cell from which
     * the legal move was requested. Must just not be equal to any single cell
     * at this initialisation point of the code.
     */
    b->legal_move_iterator_lastcell = 0xFFFFULL; 

    /* create the initial bitboard */
    int r, i, cell;
    cell = 0;

    int rstart = 0;
    int rstop = 64;
    int rinc = 8;
    int istart = 7;
    int iend = 0;
    if (reverse_ranks) {
        rstart = 63;
        rstop = -1;
        rinc = -8;
        istart = 0;
        iend = 7;
    }

    r = rstart;
    while (r != rstop) {
        for (i=r+istart; i >= r-iend; i--) { /* scan each rank from left to right */

            void *piece_addr = chessboard_base + chessboard_element_size * i;
            PieceType t = (*func_type_mapper)(piece_addr);

            int internal_cell = 63 - cell;
            if (t != PIECE_NONE) {

                if (WHITE_KING == t && internal_cell == _CELL_WHITE_KING_HOME) {
                    b->white_castling_rights = 0x44ULL;
                }
                else if (BLACK_KING == t && internal_cell == _CELL_BLACK_KING_HOME) {
                    b->black_castling_rights = 0x4400000000000000ULL;
                }

                /* place the piece in the positional bitboard */
                b->position[t] = b->position[t] | (1ULL << internal_cell);
                b->pieces_addr[internal_cell] = piece_addr;
            }
            else {
                b->pieces_addr[internal_cell] = NULL;
            }
            b->piece_type[internal_cell] = t;

            cell++;
        }

        r+=rinc;
    }

    /* clear castling rights if rooks are not in place */
    if (WHITE_ROOK != b->piece_type[0]) b->white_castling_rights &= ~0x4ULL;
    if (WHITE_ROOK != b->piece_type[7]) b->white_castling_rights &= ~0x40ULL;
    if (BLACK_ROOK != b->piece_type[56]) b->black_castling_rights &= ~0x400000000000000ULL;
    if (BLACK_ROOK != b->piece_type[63]) b->black_castling_rights &= ~0x4000000000000000ULL;

    /* return it */
    return b;
}

void destroy_bitboard(Bitboard *bitboard) 
{
	free(bitboard);
}

char *bitboard_piece_name(PieceType t)
{
    char *piece_type_name[] = {
        "WHITE PAWN",
        "WHITE KNIGHT",
        "WHITE BISHOP",
        "WHITE ROOK",
        "WHITE QUEEN",
        "WHITE KING",
        "BLACK PAWN",
        "BLACK KNIGHT",
        "BLACK BISHOP",
        "BLACK ROOK",
        "BLACK QUEEN",
        "BLACK KING"
    };
    return piece_type_name[t];
}

void print_chessboard_move(Bitboard *b, Move *m)
{
    char piece_repr[] = {
        'P',
        'N',
        'B',
        'R',
        'Q',
        'K',
        'p',
        'n',
        'b',
        'r',
        'q',
        'k',
        '@',
        '-'
    };
    int row;
    int col;
    int cell;
    char rank = 'A';
    char file = '8';
    PieceType t;
    for (row=7; row>=0; row--) {
        printf("%c", file--);
        for (col=0; col<8; col++) {
            cell = _CELL(row, col);
            t = b->piece_type[cell];

            if (NULL != m && m->from_rank == row && m->from_file == col) {
                printf(" \033[31m%c\033[0m", piece_repr[t]);
            }
            else if (NULL != m && m->to_rank == row && m->to_file == col) {
                printf(" \033[34m%c\033[0m", piece_repr[t]);
            }
            else {
                printf(" %c", piece_repr[t]); 
            }
        }
        printf("\n");
    }

    int i;
    printf(" ");
    for (i=0; i<8; i++) {
        printf(" %c", rank++);
    }
    printf("\n\n");

}

void print_chessboard(Bitboard *b)
{
    print_chessboard_move(b, NULL);
}

void print_bitboard(Bitboard *b)
{
    int i;
    printf(" - - - position bitboards - - - \n");
    for (i=0; i<PIECE_TYPE_COUNT; i++) {
        printf("%s)\n", bitboard_piece_name(i));
        print_bits(b->position[i]);
    }
    printf(" - - - enpassant rights - - - \n");
    print_bits(b->enpassant_rights);
}

void print_bits(U64 bits)
{
    U64 bits_w = bits;
    char rank = 'A';
    char file = '8';
    int nbits = 64;
    while(nbits) {
        nbits-=8;
        printf("%c ", file--);
        printf("%c ", 0x100000000000000ULL  & bits_w ? '1' : '-');
        printf("%c ", 0x200000000000000ULL  & bits_w ? '1' : '-');
        printf("%c ", 0x400000000000000ULL  & bits_w ? '1' : '-');
        printf("%c ", 0x800000000000000ULL  & bits_w ? '1' : '-');
        printf("%c ", 0x1000000000000000ULL & bits_w ? '1' : '-');
        printf("%c ", 0x2000000000000000ULL & bits_w ? '1' : '-');
        printf("%c ", 0x4000000000000000ULL & bits_w ? '1' : '-');
        printf("%c ", 0x8000000000000000ULL & bits_w ? '1' : '-');
        bits_w <<= 8;
        printf ("\n");
    }
    int i;
    printf(" ");
    for (i=0; i<8; i++) {
        printf(" %c", rank++);
    }
    printf("\n");
}

U64 bitboard_get_white_positions(Bitboard *b)
{
    U64 positions = 0ULL;
    int i;
    for (i=WHITE_PAWN; i<=WHITE_KING; i++)
        positions |= b->position[i];
    return positions;
}

U64 bitboard_get_black_positions(Bitboard *b)
{
    U64 positions = 0ULL;
    int i;
    for (i=BLACK_PAWN; i<=BLACK_KING; i++)
        positions |= b->position[i];
    return positions;
}

U64 bitboard_get_all_positions(Bitboard *b)
{
    return (bitboard_get_black_positions(b) | bitboard_get_white_positions(b));
}


PieceType get_piece_type(Bitboard *b, FileType file, RankType rank)
{
    return b->piece_type[_CELL(rank,file)];
}

void *get_piece_addr(Bitboard *b, FileType file, RankType rank)
{
    return b->pieces_addr[_CELL(rank,file)];
}

U64 get_rook_attacks(Bitboard *b, FileType file, RankType rank, U64 piece_pos) 
{
    U64 occupancy = bitboard_get_all_positions(b);
    U64 rook_file_mask = _mask_file(file) & ~piece_pos;

    /* horizontal attacks */
    U64 result = _mask_rank(rank)
        & ( (occupancy - (2 * piece_pos)) 
            ^ _mirror( _mirror(occupancy) - (2 * _mirror(piece_pos))) 
        );

    /* vertical attacks */
    U64 rook_forward = occupancy & rook_file_mask;
    U64 rook_reverse = BSWAP_64(rook_forward);
    rook_forward -= (piece_pos); 
    rook_reverse -= BSWAP_64(piece_pos);
    rook_forward ^= BSWAP_64(rook_reverse);

    return result | (rook_forward & rook_file_mask);
}

U64 get_bishop_attacks(Bitboard *b, FileType file, RankType rank, U64 piece_pos) 
{
    U64 occupancy = bitboard_get_all_positions(b);
    U64 bishop_diagonal_mask = _mask_diag(DIAGONAL(rank, file)) & ~piece_pos; 
    U64 bishop_antidiagonal_mask = _mask_antidiag(ANTI_DIAGONAL(rank, file)) & ~piece_pos;

    U64 bishop_forward, bishop_reverse;
    U64 result;
    bishop_forward = occupancy & bishop_diagonal_mask;
    bishop_reverse = BSWAP_64(bishop_forward);
    bishop_forward -= (piece_pos); 
    bishop_reverse -= BSWAP_64(piece_pos);
    bishop_forward ^= BSWAP_64(bishop_reverse);
    result = bishop_forward & bishop_diagonal_mask;
    
    bishop_forward = occupancy & bishop_antidiagonal_mask;
    bishop_reverse = BSWAP_64(bishop_forward);
    bishop_forward -= (piece_pos); 
    bishop_reverse -= BSWAP_64(piece_pos);
    bishop_forward ^= BSWAP_64(bishop_reverse);
    result |= (bishop_forward & bishop_antidiagonal_mask);
    

    return result;
}

U64 get_legal_moves(Bitboard *b, FileType file, RankType rank) 
{
    PieceType t = get_piece_type(b, file, rank);
    U64 piece_pos = b->position[t] & _mask_cell(file, rank);
    U64 result = 0ULL;            

    /* king/knight */
    U64 pclip_a = piece_pos & _clear_file(FILE_A);
    U64 pclip_h = piece_pos & _clear_file(FILE_H);
    U64 pclip_b = piece_pos & _clear_file(FILE_B);
    U64 pclip_g = piece_pos & _clear_file(FILE_G);

    U64 pawn_attacks_mask;
    U64 piece_along_the_longstep;

    switch (t) {
        case WHITE_PAWN:
            /* pawn attacks */
            piece_along_the_longstep = (bitboard_get_all_positions(b) & _mask_cell(file, rank+1));
            pawn_attacks_mask = (pclip_a << 7) | (pclip_h << 9);
            pawn_attacks_mask &= (bitboard_get_black_positions(b) | b->enpassant_rights);
            
            /* pawn movements */
            result =
                ( (piece_pos << 8) 
                    | ((b->white_remaining_pawns_longsteps 
                        & piece_pos
                        & (~piece_along_the_longstep >> 8)
                    ) << 16 )
                ) & (~bitboard_get_black_positions(b));

            result |= pawn_attacks_mask;
            break;
        case BLACK_PAWN:
            /* pawn attacks */
            piece_along_the_longstep = (bitboard_get_all_positions(b) & _mask_cell(file, rank-1));
            pawn_attacks_mask = (pclip_h >> 7) | (pclip_a >> 9);
            pawn_attacks_mask &= (bitboard_get_white_positions(b) | b->enpassant_rights); 
            
            /* pawn movements */
            result =
                ( (piece_pos >> 8)                          /* can move forward */
                    | ((b->black_remaining_pawns_longsteps  /* or forward by two */
                        & piece_pos                         /* if the piece is in the initial position */
                        & (~piece_along_the_longstep << 8)  /* and no pawn is along the way */
                      ) >> 16)
                ) & (~bitboard_get_white_positions(b));

            result |= pawn_attacks_mask;
            break;

        case WHITE_KING:
            result = (pclip_a >> 1) 
               | (pclip_a >> 9)
               | (pclip_a << 7)
               | (pclip_h << 1)
               | (pclip_h << 9)
               | (pclip_h >> 7)
               | (piece_pos << 8)
               | (piece_pos >> 8)
               | b->white_castling_rights;
            break;
        case BLACK_KING:
            result = (pclip_a >> 1) 
               | (pclip_a >> 9)
               | (pclip_a << 7)
               | (pclip_h << 1)
               | (pclip_h << 9)
               | (pclip_h >> 7)
               | (piece_pos << 8)
               | (piece_pos >> 8)
               | b->black_castling_rights;
            break;

        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            result = (pclip_g & pclip_h) >> 6
               | (pclip_b & pclip_a) >> 10
               | (pclip_h) >> 15
               | (pclip_a) >> 17
               | (pclip_b & pclip_a) << 6
               | (pclip_g & pclip_h) << 10
               | (pclip_a) << 15
               | (piece_pos) << 17;
            break;

        case WHITE_ROOK:
        case BLACK_ROOK:
            result = get_rook_attacks(b, file, rank, piece_pos);
            break;

        case WHITE_BISHOP:
        case BLACK_BISHOP:
            result = get_bishop_attacks(b, file, rank, piece_pos);
            break;

        case WHITE_QUEEN:
        case BLACK_QUEEN:
            result = get_bishop_attacks(b, file, rank, piece_pos) 
                | get_rook_attacks(b, file, rank, piece_pos);
            break;
    }
    
    /* reset own color bits */
    if (piece_pos & bitboard_get_white_positions(b))
        result &= ~bitboard_get_white_positions(b);
    else
        result &= ~bitboard_get_black_positions(b);

    return result;
}

int is_legal_move(Bitboard *b, Move *m)
{
    U64 legal_moves = get_legal_moves(b, m->from_file, m->from_rank);
    return !!(legal_moves & _mask_cell(m->to_file, m->to_rank));
}

void _perform_piece_move(Bitboard *b, Move *m)
{
    PieceType t = get_piece_type(b, m->from_file, m->from_rank);
    PieceType ttarget = get_piece_type(b, m->to_file, m->to_rank);
    PieceType ttarget_new = (PIECE_NONE == m->promote_to) ? t : m->promote_to;

    /* move to position */
    b->position[t] &= (~_mask_cell(m->from_file, m->from_rank));       /* remove piece from original square */
    b->position[ttarget_new] |= (_mask_cell(m->to_file, m->to_rank));  /* place piece to target square */

    /* the target piece type may be empty for example for en-passant captures */
    if (ttarget != PIECE_NONE) {
        b->position[ttarget] &= (~_mask_cell(m->to_file, m->to_rank)); /* clear from the capture piece in case */
    }
    else {
        /* the piece simply moved or captured en-passant */
    }

    /* update piece_type_mapping */
    b->piece_type[_CELL(m->from_rank, m->from_file)] = PIECE_NONE;
    b->piece_type[_CELL(m->to_rank, m->to_file)] = ttarget_new;

    /* update original piece position */
    void *piece_addr_from = b->pieces_addr[_CELL(m->from_rank, m->from_file)];
    b->pieces_addr[_CELL(m->from_rank, m->from_file)] = NULL;
    b->pieces_addr[_CELL(m->to_rank, m->to_file)] = piece_addr_from;
}

void bitboard_do_move(Bitboard *b, Move *m)
{
    Move rook_move;
    PieceType t = get_piece_type(b, m->from_file, m->from_rank);
    PieceType ttarget = get_piece_type(b, m->to_file, m->to_rank);
    int cell_target = _CELL(m->to_rank, m->to_file);

    U64 piece_pos = (b->position[t] & _mask_cell(m->from_file, m->from_rank));
    U64 longsteps_old;

    /* clear enpassant chances (they'll be set later if necessary) */
    b->enpassant_rights = 0x0ULL;

    switch (t) {
        case WHITE_PAWN:
            longsteps_old = b->white_remaining_pawns_longsteps;

            /* clear available longsteps for the pawn of this color */
            b->white_remaining_pawns_longsteps &= ~piece_pos;

            if (longsteps_old != b->white_remaining_pawns_longsteps) {
                /* 
                 * This pawn may have moved of two positions! enable enpassant
                 * chances by turning on the en-passant bit as if the pawn moved of one
                 * position.
                 */ 
                b->enpassant_rights = (piece_pos << 8);
            }
            
            /* 
             * If this pawn moved diagonally, and the target square is empty,
             * it was an en-passant capture! 
             */
            if (m->to_file != m->from_file && ttarget == PIECE_NONE) {
                /* Clear out captured pawn behind the target*/
                b->position[BLACK_PAWN] &= ~(_mask_cell(m->to_file, m->to_rank-1));
                b->piece_type[cell_target - 8] = PIECE_NONE;
                b->pieces_addr[cell_target - 8] = NULL;
            }
            
            break;
        case BLACK_PAWN:
            longsteps_old = b->black_remaining_pawns_longsteps;

            /* clear available longsteps for the pawn of this color */
            b->black_remaining_pawns_longsteps &= ~piece_pos;

            if (longsteps_old != b->black_remaining_pawns_longsteps) {
                /* enable enpassant chances (see comment for white). */ 
                b->enpassant_rights = (piece_pos >> 8);
            }
            
            if (m->to_file != m->from_file && ttarget == PIECE_NONE) {
                /* Clear out captured pawn behind the target*/
                b->position[WHITE_PAWN] &= ~(_mask_cell(m->to_file, m->to_rank+1));
                b->piece_type[cell_target + 8] = PIECE_NONE;
                b->pieces_addr[cell_target + 8] = NULL;
            }
            break;
        case WHITE_KING:
            /* 
             * The followings are to move the rook next to the king in case of
             * castling.
             */
            if (_CELL_WHITE_KING_LEFTCASTLE == cell_target
                && (MASK_WHITE_KING_LEFT_CASTLE & b->white_castling_rights)) {

                rook_move.from_file = FILE_A; 
                rook_move.from_rank = RANK_1;
                rook_move.to_file =   FILE_D;
                rook_move.to_rank =   RANK_1;
                rook_move.promote_to = PIECE_NONE;
                _perform_piece_move(b, &rook_move);
            }
            else if (_CELL_WHITE_KING_RIGHTCASTLE == cell_target
                && (MASK_WHITE_KING_RIGHT_CASTLE & b->white_castling_rights)) {

                rook_move.from_file = FILE_H; 
                rook_move.from_rank = RANK_1;
                rook_move.to_file =   FILE_F;
                rook_move.to_rank =   RANK_1;
                rook_move.promote_to = PIECE_NONE;
                _perform_piece_move(b, &rook_move);
            }

            b->white_castling_rights = 0x0ULL;

            break;

        case BLACK_KING:

            /* 
             * The followings are to move the rook next to the king in case of
             * castling.
             */
            if (_CELL_BLACK_KING_LEFTCASTLE == cell_target
                && (MASK_BLACK_KING_LEFT_CASTLE & b->black_castling_rights)) {

                rook_move.from_file = FILE_A; 
                rook_move.from_rank = RANK_8;
                rook_move.to_file =   FILE_D;
                rook_move.to_rank =   RANK_8;
                rook_move.promote_to = PIECE_NONE;
                _perform_piece_move(b, &rook_move);
            }
            else if (_CELL_BLACK_KING_RIGHTCASTLE == cell_target
                && (MASK_BLACK_KING_RIGHT_CASTLE & b->black_castling_rights)) {
                rook_move.from_file = FILE_H; 
                rook_move.from_rank = RANK_8;
                rook_move.to_file =   FILE_F;
                rook_move.to_rank =   RANK_8;
                rook_move.promote_to = PIECE_NONE;
                _perform_piece_move(b, &rook_move);
            }

            b->black_castling_rights = 0x0ULL;

            break;
        case BLACK_ROOK:
            if (m->from_file == FILE_A && m->from_rank == RANK_8) {
                /* clear left castling */
                b->black_castling_rights &= (~_mask_cell(FILE_C, RANK_8));
            }
            else if (m->from_file == FILE_H && m->from_rank == RANK_8) {
                /* clear right castling */
                b->black_castling_rights &= (~_mask_cell(FILE_G, RANK_8));
            }
            break;
        case WHITE_ROOK:
            if (m->from_file == FILE_A && m->from_rank == RANK_1) {
                /* clear left castling */
                b->white_castling_rights &= (~_mask_cell(FILE_C, RANK_1));
            }
            else if (m->from_file == FILE_H && m->from_rank == RANK_1) {
                /* clear right castling */
                b->white_castling_rights &= (~_mask_cell(FILE_G, RANK_1));
            }
            break;
    }

    rook_move.promote_to = PIECE_NONE;
    _perform_piece_move(b, m);

}

int get_next_legal_move(Bitboard *b, Move *ptr_move_dest)
{
    FileType f = ptr_move_dest->from_file;
    RankType r = ptr_move_dest->from_rank;
    U64 fr_cell = _mask_cell(f, r);

    if ( 0x0ULL == b->legal_move_iterator) {
        if (b->legal_move_iterator_lastcell == fr_cell) {
            b->legal_move_iterator_lastcell = 0xFFULL;
            return 0;
        }
        b->legal_move_iterator = get_legal_moves(b, f, r);
        b->legal_move_iterator_lastcell = fr_cell;
    }

    U64 next_move = LS1B(b->legal_move_iterator);

    U64 next_move_copy = next_move;
    int cell_of_next_move = 0;
    while(next_move_copy >>= 1) {
        cell_of_next_move++;
    }

    if (!!(b->legal_move_iterator)) {
        /* return and clear this move */
        b->legal_move_iterator &= (~next_move);

        ptr_move_dest->to_file = _FILE(cell_of_next_move);
        ptr_move_dest->to_rank = _RANK(cell_of_next_move);
        return 1;
    }

    return 0;
}
