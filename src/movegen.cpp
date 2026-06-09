#include "movegen.h"
#include "bitboard.h"
#include <bits/stdc++.h>
using namespace std;

void generate_moves(MoveList *move_list) {
    move_list->count = 0;

    // --- 1. PAWN MOVES ---
    const int pawn = (side == white) ? P : p;
    U64 pawn_bitboard = bitboards[pawn];

    while (pawn_bitboard > 0) {
        const int source = get_lsb_index(pawn_bitboard);

        if (side == white) {
            // A. Single Push (+8 squares)
            if (const int target = source + 8; target <= h8 && !GET_BIT(occupancies[2], target)) {
                // Promotion Check (If reaching rank 8)
                if (source >= a7 && source <= h7) {
                    add_move(move_list, ENCODE_MOVE(source, target, P, Q, 0, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, target, P, R, 0, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, target, P, B, 0, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, target, P, N, 0, 0, 0, 0));
                } else {
                    add_move(move_list, ENCODE_MOVE(source, target, P, 0, 0, 0, 0, 0));
                    // B. Double Push (+16 squares from starting rank)
                    if (source >= a2 && source <= h2) {
                        if (const int double_target = source + 16; !GET_BIT(occupancies[2], double_target)) {
                            add_move(move_list, ENCODE_MOVE(source, double_target, P, 0, 0, 1, 0, 0));
                        }
                    }
                }
            }
            // C. Pawn Captures (Diagonal masking)
            U64 attacks = mask_pawn_attacks(white, source) & occupancies[black];
            while (attacks > 0) {
                const int cap_target = get_lsb_index(attacks);
                // Promotion Capture Check
                if (source >= a7 && source <= h7) {
                    add_move(move_list, ENCODE_MOVE(source, cap_target, P, Q, 1, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, cap_target, P, R, 1, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, cap_target, P, B, 1, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, cap_target, P, N, 1, 0, 0, 0));
                } else {
                    add_move(move_list, ENCODE_MOVE(source, cap_target, P, 0, 1, 0, 0, 0));
                }
                POP_BIT(attacks, cap_target);
            }
            // D. En Passant (Special capture logic)
            if (enpassant != -1 && source >= a5 && source <= h5) {
                if (const U64 ep_attacks = mask_pawn_attacks(white, source) & (1ULL << enpassant); ep_attacks > 0) {
                    add_move(move_list, ENCODE_MOVE(source, enpassant, P, 0, 1, 0, 1, 0));
                }
            }
        }
        else { // BLACK PAWN LOGIC
            // A. Single Push (-8 squares)
            if (const int target = source - 8; target >= a1 && !GET_BIT(occupancies[2], target)) {
                if (source >= a2 && source <= h2) {
                    add_move(move_list, ENCODE_MOVE(source, target, p, q, 0, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, target, p, r, 0, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, target, p, b, 0, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, target, p, n, 0, 0, 0, 0));
                } else {
                    add_move(move_list, ENCODE_MOVE(source, target, p, 0, 0, 0, 0, 0));
                    // B. Double Push (-16 squares)
                    if (source >= a7 && source <= h7) {
                        if (const int double_target = source - 16; !GET_BIT(occupancies[2], double_target)) {
                            add_move(move_list, ENCODE_MOVE(source, double_target, p, 0, 0, 1, 0, 0));
                        }
                    }
                }
            }
            // C. Pawn Captures
            U64 attacks = mask_pawn_attacks(black, source) & occupancies[white];
            while (attacks > 0) {
                const int cap_target = get_lsb_index(attacks);
                if (source >= a2 && source <= h2) {
                    add_move(move_list, ENCODE_MOVE(source, cap_target, p, q, 1, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, cap_target, p, r, 1, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, cap_target, p, b, 1, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, cap_target, p, n, 1, 0, 0, 0));
                } else {
                    add_move(move_list, ENCODE_MOVE(source, cap_target, p, 0, 1, 0, 0, 0));
                }
                POP_BIT(attacks, cap_target);
            }
            // D. En Passant
            if (enpassant != -1 && source >= a4 && source <= h4) {
                if (const U64 ep_attacks = mask_pawn_attacks(black, source) & (1ULL << enpassant); ep_attacks > 0) {
                    add_move(move_list, ENCODE_MOVE(source, enpassant, p, 0, 1, 0, 1, 0));
                }
            }
        }
        POP_BIT(pawn_bitboard, source);
    }

    // --- 2. KNIGHT MOVES ---
    const int knight = (side == white) ? N : n;
    U64 knight_bitboard = bitboards[knight];

    while (knight_bitboard > 0) {
        const int source = get_lsb_index(knight_bitboard);
        U64 attacks = mask_knight_attacks(source) & ~(side == white ? occupancies[white] : occupancies[black]);

        while (attacks > 0) {
            const int target = get_lsb_index(attacks);
            const int capture = GET_BIT((side == white ? occupancies[black] : occupancies[white]), target) ? 1 : 0;
            add_move(move_list, ENCODE_MOVE(source, target, knight, 0, capture, 0, 0, 0));
            POP_BIT(attacks, target);
        }
        POP_BIT(knight_bitboard, source);
    }

    // --- 3. KING MOVES ---
    const int king = (side == white) ? K : k;
    U64 king_bitboard = bitboards[king];

    while (king_bitboard > 0) {
        const int source = get_lsb_index(king_bitboard);
        U64 attacks = mask_king_attacks(source) & ~(side == white ? occupancies[white] : occupancies[black]);

        while (attacks > 0) {
            const int target = get_lsb_index(attacks);
            const int capture = GET_BIT((side == white ? occupancies[black] : occupancies[white]), target) ? 1 : 0;
            add_move(move_list, ENCODE_MOVE(source, target, king, 0, capture, 0, 0, 0));
            POP_BIT(attacks, target);
        }
        POP_BIT(king_bitboard, source);
    }

    // --- 4. BISHOP MOVES ---
    const int bishop = (side == white) ? B : b;
    U64 bishop_bitboard = bitboards[bishop];

    while (bishop_bitboard > 0) {
        const int source = get_lsb_index(bishop_bitboard);
        // Note: Sliding pieces need the global occupancy (occupancies[2]) to know where to stop sliding!
        U64 attacks = mask_bishop_attacks(source, occupancies[2]) & ~(side == white ? occupancies[white] : occupancies[black]);

        while (attacks > 0) {
            const int target = get_lsb_index(attacks);
            const int capture = GET_BIT((side == white ? occupancies[black] : occupancies[white]), target) ? 1 : 0;
            add_move(move_list, ENCODE_MOVE(source, target, bishop, 0, capture, 0, 0, 0));
            POP_BIT(attacks, target);
        }
        POP_BIT(bishop_bitboard, source);
    }

    // --- 5. ROOK MOVES ---
    const int rook = (side == white) ? R : r;
    U64 rook_bitboard = bitboards[rook];

    while (rook_bitboard > 0) {
        const int source = get_lsb_index(rook_bitboard);
        U64 attacks = mask_rook_attacks(source, occupancies[2]) & ~(side == white ? occupancies[white] : occupancies[black]);

        while (attacks > 0) {
            const int target = get_lsb_index(attacks);
            const int capture = GET_BIT((side == white ? occupancies[black] : occupancies[white]), target) ? 1 : 0;
            add_move(move_list, ENCODE_MOVE(source, target, rook, 0, capture, 0, 0, 0));
            POP_BIT(attacks, target);
        }
        POP_BIT(rook_bitboard, source);
    }

    // --- 6. QUEEN MOVES ---
    const int queen = (side == white) ? Q : q;
    U64 queen_bitboard = bitboards[queen];

    while (queen_bitboard > 0) {
        const int source = get_lsb_index(queen_bitboard);
        U64 attacks = mask_queen_attacks(source, occupancies[2]) & ~(side == white ? occupancies[white] : occupancies[black]);

        while (attacks > 0) {
            const int target = get_lsb_index(attacks);
            const int capture = GET_BIT((side == white ? occupancies[black] : occupancies[white]), target) ? 1 : 0;
            add_move(move_list, ENCODE_MOVE(source, target, queen, 0, capture, 0, 0, 0));
            POP_BIT(attacks, target);
        }
        POP_BIT(queen_bitboard, source);
    }

    // --- 7. CASTLING MOVES ---
    if (side == white) {
        // Kingside Castling (Short)
        if (castle & wss) {
            // Check if f1 (5) and g1 (6) are empty
            if (!GET_BIT(occupancies[2], f1) && !GET_BIT(occupancies[2], g1)) {
                // The King cannot be in check, and cannot walk through check
                if (!is_square_attacked(e1, black) && !is_square_attacked(f1, black) && !is_square_attacked(g1, black)) {
                    // Suitcase: source, target, piece, promoted, capture=0, double=0, ep=0, castle=1
                    add_move(move_list, ENCODE_MOVE(e1, g1, K, 0, 0, 0, 0, 1));
                }
            }
        }
        // Queenside Castling (Long)
        if (castle & wl) {
            // Check if d1 (3), c1 (2), b1 (1) are empty
            if (!GET_BIT(occupancies[2], d1) && !GET_BIT(occupancies[2], c1) && !GET_BIT(occupancies[2], b1)) {
                if (!is_square_attacked(e1, black) && !is_square_attacked(d1, black) && !is_square_attacked(c1, black)) {
                    add_move(move_list, ENCODE_MOVE(e1, c1, K, 0, 0, 0, 0, 1));
                }
            }
        }
    } else {
        // Kingside Castling (Black Short)
        if (castle & bs) {
            // Check if f8 (61) and g8 (62) are empty
            if (!GET_BIT(occupancies[2], f8) && !GET_BIT(occupancies[2], g8)) {
                if (!is_square_attacked(e8, white) && !is_square_attacked(f8, white) && !is_square_attacked(g8, white)) {
                    add_move(move_list, ENCODE_MOVE(e8, g8, k, 0, 0, 0, 0, 1));
                }
            }
        }
        // Queenside Castling (Black Long)
        if (castle & bl) {
            // Check if d8 (59), c8 (58), b8 (57) are empty
            if (!GET_BIT(occupancies[2], d8) && !GET_BIT(occupancies[2], c8) && !GET_BIT(occupancies[2], b8)) {
                if (!is_square_attacked(e8, white) && !is_square_attacked(d8, white) && !is_square_attacked(c8, white)) {
                    add_move(move_list, ENCODE_MOVE(e8, c8, k, 0, 0, 0, 0, 1));
                }
            }
        }
    }
}