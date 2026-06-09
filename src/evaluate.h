#pragma once
#include "types.h"

// Base piece values in centipawns (100 = 1 pawn, 300 = 1 knight)
const int material_score[12] = {
    100, 300, 300, 500, 900, 10000,      // White: P, N, B, R, Q, K
    -100, -300, -300, -500, -900, -10000 // Black: p, n, b, r, q, k
};

// --- CORRECTED PAWN TABLE ---
// Pawns get bonuses for marching forward towards Rank 8 (Promotion)
const int pawn_pst[64] = {
     0,  0,  0,  0,  0,  0,  0,  0, // Rank 1
     5, 10, 10,-20,-20, 10, 10,  5, // Rank 2 (Starting position)
     5, -5,-10,  0,  0,-10, -5,  5, // Rank 3
     0,  0,  0, 20, 20,  0,  0,  0, // Rank 4
     5,  5, 10, 25, 25, 10,  5,  5, // Rank 5
    10, 10, 20, 30, 30, 20, 10, 10, // Rank 6
    50, 50, 50, 50, 50, 50, 50, 50, // Rank 7 (Huge bonus for passed pawns)
     0,  0,  0,  0,  0,  0,  0,  0  // Rank 8 (Promotion)
};

// --- CORRECTED KNIGHT TABLE ---
// Knights want to be in the center of the board (-50 penalty on the edges)
const int knight_pst[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50, // Rank 1
    -40,-20,  0,  5,  5,  0,-20,-40, // Rank 2
    -30,  5, 10, 15, 15, 10,  5,-30, // Rank 3
    -30,  0, 15, 20, 20, 15,  0,-30, // Rank 4
    -30,  5, 15, 20, 20, 15,  5,-30, // Rank 5
    -30,  0, 10, 15, 15, 10,  0,-30, // Rank 6
    -40,-20,  0,  5,  5,  0,-20,-40, // Rank 7
    -50,-40,-30,-30,-30,-30,-40,-50  // Rank 8
};

// --- CORRECTED BISHOP TABLE ---
// Bishops want active central diagonals
const int bishop_pst[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20, // Rank 1
    -10,  5,  0,  0,  0,  0,  5,-10, // Rank 2
    -10, 10, 10, 10, 10, 10, 10,-10, // Rank 3
    -10,  0, 10, 10, 10, 10,  0,-10, // Rank 4
    -10,  5,  5, 10, 10,  5,  5,-10, // Rank 5
    -10,  0,  5, 10, 10,  5,  0,-10, // Rank 6
    -10,  0,  0,  0,  0,  0,  0,-10, // Rank 7
    -20,-10,-10,-10,-10,-10,-10,-20  // Rank 8
};

// --- CORRECTED ROOK TABLE ---
// Rooks want to control Rank 7 and stay centralized on Rank 1
const int rook_pst[64] = {
      0,  0,  0,  5,  5,  0,  0,  0, // Rank 1 (Centralized)
     -5,  0,  0,  0,  0,  0,  0, -5, // Rank 2
     -5,  0,  0,  0,  0,  0,  0, -5, // Rank 3
     -5,  0,  0,  0,  0,  0,  0, -5, // Rank 4
     -5,  0,  0,  0,  0,  0,  0, -5, // Rank 5
     -5,  0,  0,  0,  0,  0,  0, -5, // Rank 6
      5, 10, 10, 10, 10, 10, 10,  5, // Rank 7 ("Pigs on the 7th")
      0,  0,  0,  0,  0,  0,  0,  0  // Rank 8
};

// --- CORRECTED KING TABLE ---
// King desperately wants to hide behind pawns by castling into the corners of Rank 1
const int king_pst[64] = {
     20, 30, 10,  0,  0, 10, 30, 20, // Rank 1 (g1 and c1 get huge +30 bonuses)
     20, 20,  0,  0,  0,  0, 20, 20, // Rank 2
    -10,-20,-20,-20,-20,-20,-20,-10, // Rank 3
    -20,-30,-30,-40,-40,-30,-30,-20, // Rank 4
    -30,-40,-40,-50,-50,-40,-40,-30, // Rank 5
    -30,-40,-40,-50,-50,-40,-40,-30, // Rank 6
    -30,-40,-40,-50,-50,-40,-40,-30, // Rank 7
    -30,-40,-40,-50,-50,-40,-40,-30  // Rank 8 (Terrible penalty, do not walk into the enemy camp!)
};

int evaluate_position();