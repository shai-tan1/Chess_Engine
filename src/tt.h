#pragma once
#include "types.h"

// Alpha-Beta Flags for the TT
#define HASH_EXACT 0 // Exact evaluation score
#define HASH_ALPHA 1 // Upper bound (fail-low)
#define HASH_BETA  2 // Lower bound (fail-high)

// The memory footprint for a single position
struct TT_Entry {
    U64 hash_key;
    int depth;
    int flag;
    int score;
    int best_move;
};

// Global pointers and sizes
extern TT_Entry* tt;
extern int tt_size;

// Core functions
void init_tt(int megabytes);
void clear_tt();
int read_tt(int depth, int alpha, int beta, int* best_move);
void write_tt(int depth, int score, int flag, int best_move);