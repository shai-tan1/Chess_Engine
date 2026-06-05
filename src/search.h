#pragma once
#include "types.h"

// Global variable to store the best move found during calculation
extern int best_move;

// Entry point for the search engine. Returns the evaluation score of the best move.
int search_position(int depth);