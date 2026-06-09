#pragma once
// #pragma once ensures this header is only included once during compilation
#include <bits/stdc++.h>
using namespace std;
typedef uint64_t U64; // Creates a shorthand for 64-bit unsigned integers

// --- SQUARES ---
// Maps the 64 squares of the board to indices 0 through 63.
// a1 is assigned 0, b1 is 1, all the way to h8 which is 63.
enum Square {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

// --- COLORS ---
// Colors are assigned as 0 for white and 1 for black.
// This allows us to use bitwise XOR (^ 1) to easily swap turns.
enum Color {
    white,
    black
};

// --- PIECES ---
// Unique identifiers for all 12 piece types.
// Capital letters for White (0-5), lowercase for Black (6-11).
enum Piece {
    P, N, B, R, Q, K,
    p, n, b, r, q, k
};

// --- CASTLING RIGHTS ---
// Uses bits to track castling. 1 = White Short, 2 = White Long, 4 = Black Short, 8 = Black Long
enum {
    wss = 1, wl = 2, bs = 4, bl = 8
};