#pragma once
// so that it is read only once
#include  <bits/stdc++.h>
using namespace std;
typedef uint64_t U64;
// The 64 squares of the board
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
//a1 assigned as 0,b1 as 1 and so on

enum Color {
    white,
    black
};
// colors are assigned as 0 and 1 for white and black

enum Piece {
    P, N, B, R, Q, K, // White pieces (0 to 5)
    p, n, b, r, q, k  // Black pieces (6 to 11)
};

// Castling rights (4 bits: bq bk wq wk)
enum {
    wss = 1, wl = 2, bs = 4, bl = 8
};
// shoert and long castle