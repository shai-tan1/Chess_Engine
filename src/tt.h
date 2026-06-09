#pragma once
#include "types.h"

// Alpha-Beta Flags for the TT
// Tells the engine if the stored score is a perfect evaluation, or just an upper/lower bound
#define HASH_EXACT 0 // Exact evaluation score
#define HASH_ALPHA 1 // Upper bound (fail-low)
#define HASH_BETA  2 // Lower bound (fail-high)

// The memory footprint for a single chess position
struct TT_Entry {
    U64 hash_key;    // The Zobrist signature of the board
    int depth;       // How deep it was searched
    int flag;        // The type of score (Exact, Alpha, Beta)
    int score;       // The evaluation in centipawns
    int best_move;   // The mathematically best move found
};

// Global pointers and array sizes
extern TT_Entry* tt;
extern int tt_size;

// Core functions
void init_tt(int megabytes);
void clear_tt();
int read_tt(int depth, int alpha, int beta, int* best_move);
void write_tt(int depth, int score, int flag, int best_move);