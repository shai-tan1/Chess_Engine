#include "tt.h"
#include "board.h"
#include <iostream>

using namespace std;

TT_Entry* tt = nullptr;
int tt_size = 0;

// Unknown score constant used when a read fails (position wasn't in memory)
const int unknown_score = -1000000;

// Allocate memory dynamically based on user/GUI settings
void init_tt(int megabytes) {
    int hash_size = 0x100000 * megabytes; // 1 MB = 0x100000 bytes
    tt_size = hash_size / sizeof(TT_Entry); // Calculate how many structs fit in that RAM

    // Free old memory if we are resizing
    if (tt != nullptr) {
        delete[] tt;
    }

    tt = new TT_Entry[tt_size]; // Allocate the massive array
    clear_tt();
    cout << "info string Transposition Table initialized with " << megabytes << " MB\n";
}

// Reset the table for a new game
void clear_tt() {
    for (int i = 0; i < tt_size; i++) {
        tt[i].hash_key = 0;
        tt[i].depth = 0;
        tt[i].flag = 0;
        tt[i].score = 0;
        tt[i].best_move = 0;
    }
}

// Extract data from the table
int read_tt(int depth, int alpha, int beta, int* best_move) {
    U64 hash = generate_hash_key();
    // Modulo arithmetic gives us an instant index lookup (O(1) time complexity)
    TT_Entry* entry = &tt[hash % tt_size];

    // Hash collision check: Verify it is the exact same board position
    if (entry->hash_key == hash) {
        // Always extract the best move (we use this for the Hash Move ordering heuristic)
        *best_move = entry->best_move;

        // Only trust the score if the previous search was deeper or equal to what we need now
        if (entry->depth >= depth) {
            if (entry->flag == HASH_EXACT) return entry->score;
            if (entry->flag == HASH_ALPHA && entry->score <= alpha) return alpha;
            if (entry->flag == HASH_BETA && entry->score >= beta) return beta;
        }
    }

    return unknown_score;
}

// Save data to the table
void write_tt(int depth, int score, int flag, int best_move) {
    U64 hash = generate_hash_key();
    TT_Entry* entry = &tt[hash % tt_size]; // "Always Replace" overwrite scheme
    
    entry->hash_key = hash;
    entry->depth = depth;
    entry->flag = flag;
    entry->score = score;
    entry->best_move = best_move;
}