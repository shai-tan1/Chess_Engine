#include "evaluate.h"
#include "bitboard.h"

int evaluate_position() {
    int score = 0;

    // --- 1. MATERIAL EVALUATION ---
    for (int piece = P; piece <= k; piece++) {
        int piece_count = count_bits(bitboards[piece]);
        score += piece_count * material_score[piece];
    }

    // --- 2. POSITIONAL EVALUATION ---
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
        score -= pawn_pst[square ^ 56];
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

    // Bishops
    bitboard_copy = bitboards[B];
    while (bitboard_copy) {
        square = get_lsb_index(bitboard_copy);
        score += bishop_pst[square];
        POP_BIT(bitboard_copy, square);
    }
    bitboard_copy = bitboards[b];
    while (bitboard_copy) {
        square = get_lsb_index(bitboard_copy);
        score -= bishop_pst[square ^ 56];
        POP_BIT(bitboard_copy, square);
    }

    // Rooks
    bitboard_copy = bitboards[R];
    while (bitboard_copy) {
        square = get_lsb_index(bitboard_copy);
        score += rook_pst[square];
        POP_BIT(bitboard_copy, square);
    }
    bitboard_copy = bitboards[r];
    while (bitboard_copy) {
        square = get_lsb_index(bitboard_copy);
        score -= rook_pst[square ^ 56];
        POP_BIT(bitboard_copy, square);
    }

    // Kings (This fixes the castling avoidance!)
    bitboard_copy = bitboards[K];
    while (bitboard_copy) {
        square = get_lsb_index(bitboard_copy);
        score += king_pst[square];
        POP_BIT(bitboard_copy, square);
    }
    bitboard_copy = bitboards[k];
    while (bitboard_copy) {
        square = get_lsb_index(bitboard_copy);
        score -= king_pst[square ^ 56];
        POP_BIT(bitboard_copy, square);
    }

    return (side == white) ? score : -score;
}