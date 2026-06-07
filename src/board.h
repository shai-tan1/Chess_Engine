#pragma once
#include "types.h"
#include <string>
using namespace std;

void parse_fen(const string &fen);
void print_board();
void print_bitboard(const U64 bitboard);
int parse_square(const string &sq);

// --- Zobrist Hashing & History Globals ---
extern U64 piece_keys[12][64];
extern U64 enpassant_keys[64];
extern U64 castle_keys[16];
extern U64 side_key;

extern U64 game_history[1024];
extern int history_ply;

void init_random_keys();
U64 generate_hash_key();