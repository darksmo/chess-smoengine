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

void init_move(Move *m) 
{
    m->from_file = FILE_A;
    m->from_rank = RANK_1;
    m->to_file   = FILE_H;
    m->to_rank   = RANK_7;
    m->promote_to = PIECE_NONE;
    m->is_checkmate = 0;
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

void print_move_fmt(Move *m, const char *fmt)
{
    printf(fmt,
       m->from_file + 97, m->from_rank + 49,
       m->to_file + 97, m->to_rank + 49
    );
}

void print_move(Move *m)
{
    print_move_fmt(m, "[M] %c%c - %c%c\n");
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

int bitboard_get_white_center_count(Bitboard *b) {
    U64 white_positions = bitboard_get_white_positions(b) & MASK_CENTER_4SQ;
    return _count_bits(white_positions);
}
int bitboard_get_black_center_count(Bitboard *b) {
    U64 black_positions = bitboard_get_black_positions(b) & MASK_CENTER_4SQ;
    return _count_bits(black_positions);
}

int bitboard_get_white_count(Bitboard *b) {
    U64 white_positions = bitboard_get_white_positions(b);
    return _count_bits(white_positions);
}

int bitboard_get_black_count(Bitboard *b) {
    U64 black_positions = bitboard_get_black_positions(b);
    return _count_bits(black_positions);
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

U64 get_knight_attacks(Bitboard *b, FileType file, RankType rank, U64 piece_pos) {
    U64 pclip_a = piece_pos & _clear_file(FILE_A);
    U64 pclip_h = piece_pos & _clear_file(FILE_H);
    U64 pclip_b = piece_pos & _clear_file(FILE_B);
    U64 pclip_g = piece_pos & _clear_file(FILE_G);
    return (pclip_g & pclip_h) >> 6
       | (pclip_b & pclip_a) >> 10
       | (pclip_h) >> 15
       | (pclip_a) >> 17
       | (pclip_b & pclip_a) << 6
       | (pclip_g & pclip_h) << 10
       | (pclip_a) << 15
       | (piece_pos) << 17;
}
U64 get_black_king_attacks (Bitboard *b, FileType file, RankType rank, U64 piece_pos) {
    U64 pclip_a = piece_pos & _clear_file(FILE_A);
    U64 pclip_h = piece_pos & _clear_file(FILE_H);
    return (pclip_a >> 1) 
        | (pclip_a >> 9)
        | (pclip_a << 7)
        | (pclip_h << 1)
        | (pclip_h << 9)
        | (pclip_h >> 7)
        | (piece_pos << 8)
        | (piece_pos >> 8)
        | b->black_castling_rights;
}
U64 get_white_king_attacks (Bitboard *b, FileType file, RankType rank, U64 piece_pos) {
    U64 pclip_a = piece_pos & _clear_file(FILE_A);
    U64 pclip_h = piece_pos & _clear_file(FILE_H);
    return (pclip_a >> 1) 
       | (pclip_a >> 9)
       | (pclip_a << 7)
       | (pclip_h << 1)
       | (pclip_h << 9)
       | (pclip_h >> 7)
       | (piece_pos << 8)
       | (piece_pos >> 8)
       | b->white_castling_rights;
}
U64 get_black_pawn_attacks (Bitboard *b, FileType file, RankType rank, U64 piece_pos) {
    U64 pclip_a = piece_pos & _clear_file(FILE_A);
    U64 pclip_h = piece_pos & _clear_file(FILE_H);

    /* pawn attacks */
    U64 piece_along_the_longstep = (bitboard_get_all_positions(b) & _mask_cell(file, rank-1));
    U64 pawn_attacks_mask = (pclip_h >> 7) | (pclip_a >> 9);
    pawn_attacks_mask &= (bitboard_get_white_positions(b) | b->enpassant_rights); 
    
    /* pawn movements */
    U64 result =
        ( (piece_pos >> 8)                          /* can move forward */
            | ((b->black_remaining_pawns_longsteps  /* or forward by two */
                & piece_pos                         /* if the piece is in the initial position */
                & (~piece_along_the_longstep << 8)  /* and no pawn is along the way */
              ) >> 16)
        ) & (~bitboard_get_white_positions(b));

    return result | pawn_attacks_mask;
}
U64 get_white_pawn_attacks(Bitboard *b, FileType file, RankType rank, U64 piece_pos) {
    U64 pclip_a = piece_pos & _clear_file(FILE_A);
    U64 pclip_h = piece_pos & _clear_file(FILE_H);

    /* pawn attacks */
    U64 piece_along_the_longstep = (bitboard_get_all_positions(b) & _mask_cell(file, rank+1));
    U64 pawn_attacks_mask = (pclip_a << 7) | (pclip_h << 9);
    pawn_attacks_mask &= (bitboard_get_black_positions(b) | b->enpassant_rights);
    
    /* pawn movements */
    U64 result =
        ( (piece_pos << 8) 
            | ((b->white_remaining_pawns_longsteps 
                & piece_pos
                & (~piece_along_the_longstep >> 8)
            ) << 16 )
        ) & (~bitboard_get_black_positions(b));

    return result | pawn_attacks_mask;
}
U64 get_queen_attacks(Bitboard *b, FileType file, RankType rank, U64 piece_pos) {
    return get_bishop_attacks(b, file, rank, piece_pos) 
        | get_rook_attacks(b, file, rank, piece_pos);
}

U64 get_attacks_to_square(Bitboard *b, FileType file, RankType rank, int is_opponent_white) 
{
       U64 piece_pos = _mask_cell(file, rank);
       U64 knights, kings, bishops_queens, rooks_queens;
       knights        = b->position[WHITE_KNIGHT] | b->position[BLACK_KNIGHT];
       kings          = b->position[WHITE_KING] | b->position[BLACK_KING];

       rooks_queens = bishops_queens = 
            b->position[WHITE_QUEEN] | b->position[BLACK_QUEEN];

       rooks_queens |= b->position[WHITE_ROOK] | b->position[BLACK_ROOK];
       bishops_queens  |= b->position[WHITE_BISHOP] | b->position[BLACK_BISHOP];
     
       return (get_white_pawn_attacks(b, file, rank, piece_pos) & b->position[WHITE_PAWN])
            | (get_black_pawn_attacks(b, file, rank, piece_pos) & b->position[WHITE_PAWN])
            | (get_knight_attacks(b, file, rank, piece_pos) & knights)
            | ( (get_black_king_attacks(b, file, rank, piece_pos)
                | get_white_king_attacks(b, file, rank, piece_pos)) & kings)
            | (get_bishop_attacks(b, file, rank, piece_pos) & bishops_queens)
            | (get_rook_attacks(b, file, rank, piece_pos) & rooks_queens)
       ;
}

U64 get_legal_moves(Bitboard *b, FileType file, RankType rank) 
{
    PieceType t = get_piece_type(b, file, rank);
    U64 piece_pos = b->position[t] & _mask_cell(file, rank);
    U64 result = 0ULL;            
    
    // just some caching of the position of the white pieces...
    U64 white_piece_positions = bitboard_get_white_positions(b);
    U64 black_piece_positions = bitboard_get_black_positions(b);
    int is_white_piece = !!(piece_pos & white_piece_positions);

    /*
     * if this piece didn't exist and king is in check then this piece cannot
     * move. So we remove the piece temporarily from the chessboard and see.
     */
    PieceType king_type = is_white_piece ? WHITE_KING : BLACK_KING;
    int king_cell = _cell_of_bit(b->position[king_type]);

    // check if the king is currently checked (a priori)
    U64 king_attacked_priori = 
        get_attacks_to_square(b, _FILE(king_cell), _RANK(king_cell), !is_white_piece)
            & ( is_white_piece ? ~white_piece_positions
                               : ~black_piece_positions
            );

    // check if the king is attacked after removing the piece
    b->position[t] &= ~piece_pos; // remove piece
    U64 king_attacked_posteriori = 
        get_attacks_to_square(b, _FILE(king_cell), _RANK(king_cell), !is_white_piece)
            & (is_white_piece ? ~white_piece_positions
                              : ~black_piece_positions
            );
    b->position[t] |= piece_pos; // put piece back

    switch (t) {
        case WHITE_PAWN:
            result = get_white_pawn_attacks(b, file, rank, piece_pos);
            break;
        case BLACK_PAWN:
            result = get_black_pawn_attacks(b, file, rank, piece_pos);
            break;
        case WHITE_KING:
            result = get_white_king_attacks(b, file, rank, piece_pos);
            break;
        case BLACK_KING:
            result = get_black_king_attacks(b, file, rank, piece_pos);
            break;
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            result = get_knight_attacks(b, file, rank, piece_pos);
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
            result = get_queen_attacks(b, file, rank, piece_pos);
            break;
    }
    
    // remove pieces of own color
    if (is_white_piece) 
        result &= ~white_piece_positions;
    else
        result &= ~black_piece_positions;

    /*
     * King Special case: we need to remove squares in which the king would be in check...
     */
    if (t == king_type) { 
        Move m;
        U64 actual_moves = result;

        b->position[t] &= ~piece_pos; // remove piece
        while (result) {
            result = get_next_cell_in(result, &m);
            if (get_attacks_to_square(b, m.from_file, m.from_rank, !is_white_piece) 
                & (is_white_piece ? ~white_piece_positions : ~black_piece_positions)
            ) {
                // remove cell
                actual_moves &= ~_mask_cell(m.from_file, m.from_rank);
            }
        }
        b->position[t] |= piece_pos; // put piece back
        return actual_moves;
    }

    /*
     * Special case of king attacked a priori
     */
    if (king_attacked_priori) {
        int n_attackers = _count_bits(king_attacked_priori);
        if (t != king_type && n_attackers > 1) { return 0x0ULL; }

        /*
         * legal moves of the king (many attackers to king)
         * OR legal moves of another piece (one attackers to king)
         */
        Move m;
        int m_cell;
        U64 inbetween_attacks = 0x0ULL;
        while (king_attacked_priori) { 
            king_attacked_priori = get_next_cell_in(king_attacked_priori, &m);
            m_cell = _CELL(m.from_rank, m.from_file);
            inbetween_attacks |= 
                _mask_between(m_cell, king_cell) 
                | _mask_cell(m.from_file, m.from_rank);
        }

        // king must escape
        // if (t == king_type) { 
        //     U64 potential_moves = result 
        //         & (~inbetween_attacks | _mask_cell(m.from_file, m.from_rank)); 

        //     // from the potental moves will need to remove those in which the king would be in check
        //     U64 actual_moves = potential_moves;
        //     while (potential_moves) {
        //         potential_moves = get_next_cell_in(potential_moves, &m);
        //         if (get_attacks_to_square(b, m.from_file, m.from_rank, !is_white_piece) 
        //             & (is_white_piece ? ~white_piece_positions : ~bitboard_get_black_positions(b))
        //         ) {
        //             actual_moves &= ~_mask_cell(m.from_file, m.from_rank);
        //         }
        //     }
        //     return actual_moves;
        // }

        // a piece other than the king must cover the attack
        return result & inbetween_attacks;
    }

    /*
     * Special case of king attacked a posteriori
     */
    if (king_attacked_posteriori) {
        // this piece just can't move!
        return 0x0ULL;
    }

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

/*
 * Given a 64bit integer containing the position of white/black/other pieces,
 * fills up the struct Move corresponding to the next bit 1 found, and returns
 * the input without that bit (to be used in iteration. This function is useful
 * to enumerate all the squares occupied by pieces.
 *
 * - ptr_move_result* must be a pointer to a previously initialised stuct Move.
 * - only the from_file, from_rank fields of *ptr_move_result are filled
 *
 */
U64 get_next_cell_in(U64 positions, Move *ptr_move_result)
{
    /* don't even attempt to compute the position if none is available */
    if (!positions) return positions;

    U64 mask_ls1b = LS1B(positions);
    int cell_number = _cell_of_bit(mask_ls1b);
    ptr_move_result->from_file = _FILE(cell_number);
    ptr_move_result->from_rank = _RANK(cell_number);

    return (positions & (~mask_ls1b));
}

void reset_legal_move_iterator(Bitboard *b) {
    b->legal_move_iterator = 0x0ULL;
    b->legal_move_iterator_lastcell = 0xFFFFULL;
}

int get_next_legal_move(Bitboard *b, Move *ptr_move_dest)
{
    FileType f = ptr_move_dest->from_file;
    RankType r = ptr_move_dest->from_rank;
    U64 fr_cell = _mask_cell(f, r);

    if ( 0x0ULL == b->legal_move_iterator) {
        if (b->legal_move_iterator_lastcell == fr_cell) {
            b->legal_move_iterator_lastcell = 0xFFFFULL;
            return 0;
        }
        b->legal_move_iterator = get_legal_moves(b, f, r);
        b->legal_move_iterator_lastcell = fr_cell;
    }

    /* the next move is the next bit 1 we encounter */
    U64 next_move = LS1B(b->legal_move_iterator);
    int cell_of_next_move = _cell_of_bit(next_move);

    if (!!(b->legal_move_iterator)) {
        /* return and clear this move */
        b->legal_move_iterator &= (~next_move);

        ptr_move_dest->to_file = _FILE(cell_of_next_move);
        ptr_move_dest->to_rank = _RANK(cell_of_next_move);
        return 1;
    }

    return 0;
}
