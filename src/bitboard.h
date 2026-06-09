#pragma once
#include "types.h"
#include "move.h"
#include <bits/stdc++.h>
using namespace std;

// --- BIT MANIPULATION MACROS ---
// SET_BIT: Turns a 0 into a 1 at a specific square.
#define SET_BIT(bitboard, square) ((bitboard) |= (1ULL << (square)))
// GET_BIT: Checks if there is a 1 at a specific square.
#define GET_BIT(bitboard, square) ((bitboard) & (1ULL << (square)))
// POP_BIT: Turns a 1 into a 0 at a specific square (removes a piece).
#define POP_BIT(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

// --- EDGE MASKS ---
// If a piece on the 'h' file shifts right, it might teleport to the 'a' file on the next row.
// We bitwise AND (&) with these masks to prevent pieces from wrapping around the board.
constexpr U64 not_a_file = 0xfefefefefefefefeULL;
constexpr U64 not_h_file = 0x7f7f7f7f7f7f7f7fULL;
constexpr U64 not_ab_file = 0xfcfcfcfcfcfcfcfcULL;
constexpr U64 not_gh_file = 0x3f3f3f3f3f3f3f3fULL;

// --- GLOBAL VARIABLES (State of the Game) ---
extern U64 bitboards[12];   // One bitboard for each piece type
extern U64 occupancies[3];  // Tracks all pieces: [0]=White, [1]=Black, [2]=Both
extern int side;            // Whose turn is it? white (0) or black (1)
extern int enpassant;       // The square where an en passant capture is possible (or -1)
extern int castle;          // Castling rights represented as a 4-bit number

// --- CORE FUNCTION PROTOTYPES ---
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