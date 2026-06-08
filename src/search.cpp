#include "search.h"
#include "movegen.h"
#include "bitboard.h"
#include "evaluate.h"
#include "board.h"
#include "tt.h"
#include <cstring>
#include <iostream>
#include <chrono>

using namespace std;

int best_move = 0;

// --- MOVE ORDERING ARRAYS ---
int killer_moves[2][64];
int history_moves[12][64];

// --- TIME MANAGEMENT ---
long long allocated_time = 0;
long long start_time = 0;
bool time_over = false;
int nodes_visited = 0;

long long get_time_ms() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

void check_time() {
    if (time_over) return;
    if (get_time_ms() - start_time > allocated_time) {
        time_over = true;
    }
}

// --- ADVANCED MOVE ORDERING ---
int score_move(int move, int tt_move, int depth) {
    // 1. Transposition Table Move (Highest Priority)
    if (move == tt_move) return 30000;

    int promoted = GET_MOVE_PROMOTED(move);
    // 2. Promotions (Queen promotions are game-winning)
    if (promoted) return 20000 + abs(material_score[promoted]);

    // 3. Captures (MVV-LVA)
    if (GET_MOVE_CAPTURE(move)) {
        int target = GET_MOVE_TARGET(move);
        int attacker = GET_MOVE_PIECE(move);
        int victim = P;
        for (int i = 0; i < 12; i++) {
            if (GET_BIT(bitboards[i], target)) {
                victim = i; break;
            }
        }
        return 10000 + abs(material_score[victim]) - (abs(material_score[attacker]) / 10);
    }
    // 4. Quiet Moves (Killers & History)
    else {
        if (move == killer_moves[0][depth]) return 9000; // Primary Killer
        if (move == killer_moves[1][depth]) return 8000; // Secondary Killer

        // Return historical success score
        return history_moves[GET_MOVE_PIECE(move)][GET_MOVE_TARGET(move)];
    }
}

// --- ALPHA-BETA SEARCH ---
int alpha_beta(int alpha, int beta, int depth) {
    if ((nodes_visited++ & 2047) == 0) check_time();
    if (time_over) return 0;

    // --- PROBE TRANSPOSITION TABLE ---
    int tt_move = 0;
    int tt_score = read_tt(depth, alpha, beta, &tt_move);
    if (tt_score != -1000000) {
        return tt_score;
    }

    if (depth == 0) return evaluate_position();

    // --- NULL MOVE PRUNING (NMP) ---
    int current_king = get_lsb_index(bitboards[side == white ? K : k]);
    if (depth >= 3 && !is_square_attacked(current_king, side ^ 1)) {
        int non_pawn_material = count_bits(bitboards[side == white ? Q : q]) +
                                count_bits(bitboards[side == white ? R : r]) +
                                count_bits(bitboards[side == white ? B : b]) +
                                count_bits(bitboards[side == white ? N : n]);

        if (non_pawn_material > 0) {
            int ep_copy = enpassant;
            side ^= 1;
            enpassant = -1;

            int nmp_score = -alpha_beta(-beta, -beta + 1, depth - 1 - 2);

            side ^= 1;
            enpassant = ep_copy;

            if (nmp_score >= beta) return beta; // Prune!
        }
    }

    MoveList move_list;
    generate_moves(&move_list);
    int legal_moves_count = 0;

    // Score moves using TT, Killers, and History
    for (int i = 0; i < move_list.count; i++) {
        move_list.scores[i] = score_move(move_list.moves[i], tt_move, depth);
    }

    int best_score = -500000;
    int hash_flag = HASH_ALPHA;
    int best_move_this_node = 0;

    for (int i = 0; i < move_list.count; i++) {
        int best_score_index = i;
        for (int j = i + 1; j < move_list.count; j++) {
            if (move_list.scores[j] > move_list.scores[best_score_index]) best_score_index = j;
        }
        int temp_move = move_list.moves[i]; move_list.moves[i] = move_list.moves[best_score_index]; move_list.moves[best_score_index] = temp_move;
        int temp_score = move_list.scores[i]; move_list.scores[i] = move_list.scores[best_score_index]; move_list.scores[best_score_index] = temp_score;

        U64 local_bitboards[12], local_occupancies[3];
        int local_side = side, local_enpassant = enpassant, local_castle = castle;
        memcpy(local_bitboards, bitboards, 96);
        memcpy(local_occupancies, occupancies, 24);

        if (!make_move(move_list.moves[i])) continue;
        legal_moves_count++;

        int score = -alpha_beta(-beta, -alpha, depth - 1);

        memcpy(bitboards, local_bitboards, 96);
        memcpy(occupancies, local_occupancies, 24);
        side = local_side; enpassant = local_enpassant; castle = local_castle;

        if (time_over) return 0;

        if (score > best_score) {
            best_score = score;
            best_move_this_node = move_list.moves[i];
        }
        if (score > alpha) {
            alpha = score;
            hash_flag = HASH_EXACT;
        }
        if (alpha >= beta) {
            write_tt(depth, beta, HASH_BETA, move_list.moves[i]);

            // --- SAVE KILLER AND HISTORY MOVES ---
            if (!GET_MOVE_CAPTURE(move_list.moves[i])) {
                // Shift primary killer to secondary, save new primary
                killer_moves[1][depth] = killer_moves[0][depth];
                killer_moves[0][depth] = move_list.moves[i];

                // Reward history based on search depth (deeper cutoffs are more valuable)
                history_moves[GET_MOVE_PIECE(move_list.moves[i])][GET_MOVE_TARGET(move_list.moves[i])] += depth * depth;
            }
            return beta;
        }
    }

    if (legal_moves_count == 0) {
        int king_sq = get_lsb_index(bitboards[side == white ? K : k]);
        if (is_square_attacked(king_sq, side ^ 1)) return -100000 - depth;
        else return 0;
    }

    write_tt(depth, best_score, hash_flag, best_move_this_node);
    return best_score;
}

// --- ROOT SEARCH ---
int search_root(int depth) {
    int tt_move = 0;
    read_tt(depth, -500000, 500000, &tt_move);

    MoveList move_list;
    generate_moves(&move_list);

    for (int i = 0; i < move_list.count; i++) {
        move_list.scores[i] = score_move(move_list.moves[i], tt_move, depth);
    }

    int best_score = -500000;
    int current_best_move = 0;
    int alpha = -500000;
    int beta = 500000;
    int hash_flag = HASH_ALPHA;

    for (int i = 0; i < move_list.count; i++) {
        int best_score_index = i;
        for (int j = i + 1; j < move_list.count; j++) {
            if (move_list.scores[j] > move_list.scores[best_score_index]) best_score_index = j;
        }
        int temp_move = move_list.moves[i]; move_list.moves[i] = move_list.moves[best_score_index]; move_list.moves[best_score_index] = temp_move;
        int temp_score = move_list.scores[i]; move_list.scores[i] = move_list.scores[best_score_index]; move_list.scores[best_score_index] = temp_score;

        U64 local_bitboards[12], local_occupancies[3];
        int local_side = side, local_enpassant = enpassant, local_castle = castle;
        memcpy(local_bitboards, bitboards, 96);
        memcpy(local_occupancies, occupancies, 24);

        if (!make_move(move_list.moves[i])) continue;

        if (current_best_move == 0) current_best_move = move_list.moves[i];

        bool is_repetition = false;
        U64 current_hash = generate_hash_key();
        for (int h = 0; h < history_ply; h++) {
            if (game_history[h] == current_hash) {
                is_repetition = true;
                break;
            }
        }

        int score;
        if (is_repetition) score = 0;
        else score = -alpha_beta(-beta, -alpha, depth - 1);

        memcpy(bitboards, local_bitboards, 96);
        memcpy(occupancies, local_occupancies, 24);
        side = local_side; enpassant = local_enpassant; castle = local_castle;

        if (time_over) return 0;

        if (score > best_score) {
            best_score = score;
            current_best_move = move_list.moves[i];
        }
        if (score > alpha) {
            alpha = score;
            hash_flag = HASH_EXACT;
        }
    }

    if (current_best_move != 0) {
        best_move = current_best_move;
        write_tt(depth, best_score, hash_flag, best_move);
    }
    return best_score;
}

// --- ITERATIVE DEEPENING LOOP ---
void iterative_deepening(int max_depth, long long max_time_ms) {
    start_time = get_time_ms();
    allocated_time = max_time_ms;
    time_over = false;
    nodes_visited = 0;

    // Reset Heuristics for the new search
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));

    int absolute_best_move = 0;

    search_root(1);
    if (best_move != 0) absolute_best_move = best_move;

    for (int d = 2; d <= max_depth; d++) {
        search_root(d);
        if (time_over) break;
        if (best_move != 0) absolute_best_move = best_move;

        cout << "info depth " << d << " time " << (get_time_ms() - start_time) << " nodes " << nodes_visited << "\n";
    }

    best_move = absolute_best_move;
}