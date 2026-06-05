#pragma once
#include "types.h"
#include "move.h"
#include <bits/stdc++.h>
using namespace std;

// Macros to manipulate our 64-bit integers
#define SET_BIT(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define GET_BIT(bitboard, square) ((bitboard) & (1ULL << (square)))
#define POP_BIT(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

// lets say we are at the
// a1, b1, c1, d1, e1, f1, g1, h1,
// a2, b2, c2, d2, e2, f2, g2, h2,
// a3, b3, c3, d3, e3, f3, g3, h3,
// a4, b4, c4, d4, e4, f4, g4, h4,
// a5, b5, c5, d5, e5, f5, g5, h5,
// a6, b6, c6, d6, e6, f6, g6, h6,
// a7, b7, c7, d7, e7, f7, g7, h7,
// a8, b8, c8, d8, e8, f8, g8, h8

// if we left shifted or right shifed
// it shud at max move from 1 - 2 cols, 1 - 2 rows
// if it moves from a to h it is teleportimg
// so just check if the particular square is from a ab g h or gh
// the particular files for all the columns are made 0
// so when we do a and the ans is 1 only when the cell at  a non a b gh ab  gh square is 1
constexpr U64 not_a_file = 0xfefefefefefefefeULL;
constexpr U64 not_h_file = 0x7f7f7f7f7f7f7f7fULL;
constexpr U64 not_ab_file = 0xfcfcfcfcfcfcfcfcULL;
constexpr U64 not_gh_file = 0x3f3f3f3f3f3f3f3fULL;

extern U64 bitboards[12];   // One bitboard for each piece type
extern U64 occupancies[3];  // [0]=White, [1]=Black, [2]=Both
extern int side;            // white (0) or black (1)
extern int enpassant;       // The square where en passant is possible (or -1)
extern int castle;          // Castling rights (represented as a 4-bit number)

int count_bits(U64 bitboard);
int get_lsb_index(const U64 bitboard);
bool make_move(int move);
bool is_square_attacked(int square, int side);

U64 mask_knight_attacks(int square);
U64 mask_king_attacks(int square);
U64 mask_pawn_attacks(int side, int square);
U64 mask_rook_attacks(int square, U64 occupancy);
U64 mask_bishop_attacks(int square, U64 occupancy);
U64 mask_queen_attacks(int square, U64 occupancy);