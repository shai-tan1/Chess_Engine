#include "evaluate.h"
#include "bitboard.h"

int evaluate_position() {
    int score = 0;

    // --- 1. MATERIAL EVALUATION (Count EVERYTHING) ---
    for (int piece = P; piece <= k; piece++) {
        int piece_count = count_bits(bitboards[piece]);
        score += piece_count * material_score[piece];
    }

    // --- 2. POSITIONAL EVALUATION (Piece-Square Tables) ---
    U64 bitboard_copy;
    int square;

    // Pawns
    bitboard_copy = bitboards[P];
    while (bitboard_copy) {
        square = get_lsb_index(bitboard_copy);
        score += pawn_pst[square];
        POP_BIT(bitboard_copy, square);
    }
    bitboard_copy = bitboards[p];
    while (bitboard_copy) {
        square = get_lsb_index(bitboard_copy);
        score -= pawn_pst[square ^ 56]; // Mirror vertically for black
        POP_BIT(bitboard_copy, square);
    }

    // Knights
    bitboard_copy = bitboards[N];
    while (bitboard_copy) {
        square = get_lsb_index(bitboard_copy);
        score += knight_pst[square];
        POP_BIT(bitboard_copy, square);
    }
    bitboard_copy = bitboards[n];
    while (bitboard_copy) {
        square = get_lsb_index(bitboard_copy);
        score -= knight_pst[square ^ 56];
        POP_BIT(bitboard_copy, square);
    }

    // Return score relative to the current player's turn (Negamax style)
    return (side == white) ? score : -score;
}