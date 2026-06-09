#include "perfit.h"
#include "bitboard.h"
#include "movegen.h"
#include <iostream>
#include <chrono>

using namespace std;

// This is the recursive worker function that explores every branch
long long perft_driver(int depth) {
    if (depth == 0) return 1; // Base case: 1 position found

    MoveList move_list;
    generate_moves(&move_list);

    long long nodes = 0;

    for (int i = 0; i < move_list.count; i++) {
        // --- 1. SAVE STATE BEFORE MOVE ---
        // (Since your make_move already saves state internally before verifying king safety, 
        // we need local copies here to handle the recursive unwind)
        U64 local_bitboards[12], local_occupancies[3];
        int local_side = side, local_enpassant = enpassant, local_castle = castle;
        memcpy(local_bitboards, bitboards, 96);
        memcpy(local_occupancies, occupancies, 24);

        // --- 2. ATTEMPT MOVE ---
        if (make_move(move_list.moves[i])) {
            // Move was legal! Dive deeper.
            nodes += perft_driver(depth - 1);
        }

        // --- 3. UNMAKE MOVE (RESTORE STATE) ---
        memcpy(bitboards, local_bitboards, 96);
        memcpy(occupancies, local_occupancies, 24);
        side = local_side;
        enpassant = local_enpassant;
        castle = local_castle;
    }

    return nodes;
}

// This is the formatted wrapper function that times the test
void perft_test(int depth) {
    cout << "\nPerformance Test\n";
    cout << "========================================\n";

    auto start_time = chrono::high_resolution_clock::now();

    MoveList move_list;
    generate_moves(&move_list);

    long long total_nodes = 0;

    for (int i = 0; i < move_list.count; i++) {
        U64 local_bitboards[12], local_occupancies[3];
        int local_side = side, local_enpassant = enpassant, local_castle = castle;
        memcpy(local_bitboards, bitboards, 96);
        memcpy(local_occupancies, occupancies, 24);

        if (make_move(move_list.moves[i])) {
            long long nodes = perft_driver(depth - 1);
            total_nodes += nodes;

            // Optional: Print the move and its branch count (like standard Stockfish perft)
            // cout << "Move index " << i << ": " << nodes << " nodes\n";
        }

        memcpy(bitboards, local_bitboards, 96);
        memcpy(occupancies, local_occupancies, 24);
        side = local_side;
        enpassant = local_enpassant;
        castle = local_castle;
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end_time - start_time;

    cout << "\nDepth: " << depth << "\n";
    cout << "Nodes: " << total_nodes << "\n";
    cout << "Time:  " << duration.count() << " seconds\n";
    if (duration.count() > 0) {
        cout << "NPS:   " << (long long)(total_nodes / duration.count()) << " nodes/sec\n";
    }
    cout << "========================================\n";
}