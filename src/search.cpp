#include "search.h"
#include "movegen.h"
#include "bitboard.h"
#include "evaluate.h"
#include <cstring>
#include <bits/stdc++.h>

using namespace std;

int best_move = 0;

// --- 1. MOVE ORDERING HEURISTICS ---
// Guesses how good a move is before we actually search it
int score_move(int move) {
    int score = 0;

    // A. Promotions are almost always great
    int promoted = GET_MOVE_PROMOTED(move);
    if (promoted) {
        score += 10000 + abs(material_score[promoted]);
    }

    // B. Captures (MVV-LVA: Most Valuable Victim - Least Valuable Attacker)
    if (GET_MOVE_CAPTURE(move)) {
        int target = GET_MOVE_TARGET(move);
        int attacker = GET_MOVE_PIECE(move);

        // Find out what piece is sitting on the target square (The Victim)
        int victim = P;
        for (int i = 0; i < 12; i++) {
            if (GET_BIT(bitboards[i], target)) {
                victim = i;
                break;
            }
        }

        score += 5000 + abs(material_score[victim]) - (abs(material_score[attacker]) / 10);
    }

    return score;
}

// --- 2. RECURSIVE ALPHA-BETA WORKER ---
int alpha_beta(int alpha, int beta, int depth) {

    // Base Case: Leaf node reached
    if (depth == 0) {
        return evaluate_position();
    }

    MoveList move_list;
    generate_moves(&move_list);
    int legal_moves_count = 0;

    // --- SCORE ALL MOVES ---
    for (int i = 0; i < move_list.count; i++) {
        move_list.scores[i] = score_move(move_list.moves[i]);
    }

    for (int i = 0; i < move_list.count; i++) {
        // --- SELECTION SORT (Pick the best remaining move and bring it to the front) ---
        int best_score_index = i;
        for (int j = i + 1; j < move_list.count; j++) {
            if (move_list.scores[j] > move_list.scores[best_score_index]) {
                best_score_index = j;
            }
        }
        // Swap moves
        int temp_move = move_list.moves[i];
        move_list.moves[i] = move_list.moves[best_score_index];
        move_list.moves[best_score_index] = temp_move;
        // Swap scores
        int temp_score = move_list.scores[i];
        move_list.scores[i] = move_list.scores[best_score_index];
        move_list.scores[best_score_index] = temp_score;

        // --- EXECUTE THE SORTED MOVE ---
        U64 local_bitboards[12], local_occupancies[3];
        int local_side = side, local_enpassant = enpassant, local_castle = castle;
        memcpy(local_bitboards, bitboards, 96);
        memcpy(local_occupancies, occupancies, 24);

        if (!make_move(move_list.moves[i])) {
            continue;
        }
        legal_moves_count++;

        int score = -alpha_beta(-beta, -alpha, depth - 1);

        memcpy(bitboards, local_bitboards, 96);
        memcpy(occupancies, local_occupancies, 24);
        side = local_side;
        enpassant = local_enpassant;
        castle = local_castle;

        // Fail-hard beta cutoff (Pruning)
        if (score >= beta) {
            return beta;
        }

        // Update alpha value
        if (score > alpha) {
            alpha = score;
        }
    }

    // Terminal State Processing (Checkmate / Stalemate)
    if (legal_moves_count == 0) {
        int king_sq = get_lsb_index(bitboards[side == white ? K : k]);
        if (is_square_attacked(king_sq, side ^ 1)) {
            return -100000 - depth; // Checkmate (penalize longer paths to mate)
        } else {
            return 0; // Stalemate
        }
    }

    return alpha;
}

// --- 3. ROOT SEARCH ROUTINE ---
int search_position(int depth) {
    MoveList move_list;
    generate_moves(&move_list);

    // --- SCORE ALL MOVES ---
    for (int i = 0; i < move_list.count; i++) {
        move_list.scores[i] = score_move(move_list.moves[i]);
    }

    int best_score = -500000;
    int current_best_move = 0;
    int alpha = -500000;
    int beta = 500000;

    for (int i = 0; i < move_list.count; i++) {
        // --- SELECTION SORT (Pick the best remaining move and bring it to the front) ---
        int best_score_index = i;
        for (int j = i + 1; j < move_list.count; j++) {
            if (move_list.scores[j] > move_list.scores[best_score_index]) {
                best_score_index = j;
            }
        }
        // Swap moves
        int temp_move = move_list.moves[i];
        move_list.moves[i] = move_list.moves[best_score_index];
        move_list.moves[best_score_index] = temp_move;
        // Swap scores
        int temp_score = move_list.scores[i];
        move_list.scores[i] = move_list.scores[best_score_index];
        move_list.scores[best_score_index] = temp_score;

        // --- EXECUTE THE SORTED MOVE ---
        U64 local_bitboards[12], local_occupancies[3];
        int local_side = side, local_enpassant = enpassant, local_castle = castle;
        memcpy(local_bitboards, bitboards, 96);
        memcpy(local_occupancies, occupancies, 24);

        if (!make_move(move_list.moves[i])) {
            continue;
        }

        int score = -alpha_beta(-beta, -alpha, depth - 1);

        memcpy(bitboards, local_bitboards, 96);
        memcpy(occupancies, occupancies, 24);
        side = local_side;
        enpassant = local_enpassant;
        castle = local_castle;

        if (score > best_score) {
            best_score = score;
            current_best_move = move_list.moves[i];
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    best_move = current_best_move;
    return best_score;
}