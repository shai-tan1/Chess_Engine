#pragma once
#include "types.h"

extern int best_move;

// Advanced Move Ordering Arrays
extern int killer_moves[2][64];   // 2 Killers per ply (Max depth 64)
extern int history_moves[12][64]; // Track success rate of [piece][target_square]

void iterative_deepening(int max_depth, long long max_time_ms);