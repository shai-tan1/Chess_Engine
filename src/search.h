#pragma once
#include "types.h"

extern int best_move;

// --- ADVANCED MOVE ORDERING ARRAYS ---
// Killer Moves: Quiet moves that caused a beta cutoff in sibling nodes
extern int killer_moves[2][64];   // 2 Killers per ply (Max depth 64)
// History Moves: Tracks historical success rate of moving [piece] to [target_square]
extern int history_moves[12][64];

void iterative_deepening(int max_depth, long long max_time_ms);