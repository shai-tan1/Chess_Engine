#include <bits/stdc++.h>
using namespace std;
#include "types.h"
#include "bitboard.h"

int main() {
    //
    // cout << "--- KNIGHT Attacks from d4 ---\n";
    // U64 knight_map = mask_knight_attacks(d4);
    // print_bitboard(knight_map);
    //
    // cout << "--- KING Attacks from e4 ---\n";
    // U64 king_map = mask_king_attacks(e4);
    // print_bitboard(king_map);
    //
    // cout << "--- WHITE PAWN Attacks from e2 ---\n";
    // U64 white_pawn_map = mask_pawn_attacks(white, e2);
    // print_bitboard(white_pawn_map);
    //
    // cout << "--- BLACK PAWN Attacks from d7 ---\n";
    // U64 black_pawn_map = mask_pawn_attacks(black, d7);
    // print_bitboard(black_pawn_map);
    //
    // U64 occupancy = 0ULL;
    //
    // SET_BIT(occupancy, d6); // Block Up
    // SET_BIT(occupancy, d2); // Block Down
    // SET_BIT(occupancy, b4); // Block Left
    // SET_BIT(occupancy, g4); // Block Right
    // SET_BIT(occupancy, b6); // Block Up-Left
    // SET_BIT(occupancy, f6); // Block Up-Right
    // SET_BIT(occupancy, b2); // Block Down-Left
    // SET_BIT(occupancy, f2); // Block Down-Right
    //
    // print_bitboard(occupancy);
    // U64 rook_map = mask_rook_attacks(d4, occupancy);
    // print_bitboard(rook_map);
    //
    // U64 bishop_map = mask_bishop_attacks(d4, occupancy);
    // print_bitboard(bishop_map);
    //
    // U64 queen_map = mask_queen_attacks(d4, occupancy);
    // print_bitboard(queen_map);
    //
    // memset(bitboards, 0ULL, sizeof(bitboards));
    // memset(occupancies, 0ULL, sizeof(occupancies));
    //
    // SET_BIT(bitboards[K], e1);
    // SET_BIT(bitboards[R], a1);
    // SET_BIT(bitboards[P], e4);
    //
    // SET_BIT(bitboards[k], e8);
    // SET_BIT(bitboards[q], d8);
    // SET_BIT(bitboards[p], e5);
    //
    // // 3. Print the board!
    // cout << "--- CURRENT BOARD STATE ---\n";
    // print_board();
    //
    // string start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    // parse_fen(start_position);
    //
    // cout << "--- STARTING POSITION ---\n";
    // print_board();
    // string attack_test = "8/8/8/3n4/4K2r/8/8/8";
    // parse_fen(attack_test);
    //
    // cout << "--- ATTACK TEST BOARD ---\n";
    // print_board();
    //
    // // Check if the e4 square (where our White King is) is attacked by Black
    // if (is_square_attacked(e4, black)) {
    //     cout << "The White King on e4 is IN CHECK!\n";
    // }
    // else {
    //     cout << "The White King on e4 is SAFE.\n";
    // }

    string start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    start_position = "8/P7/8/8/8/8/8/K1k5 w - b8 0 1"; // Test FEN for promotion!
    parse_fen(start_position);

    cout << "========================================\n";
    cout << "      INTERACTIVE CHESS TERMINAL        \n";
    cout << "========================================\n";

    while (true) {
        print_board();

        // 1. GENERATE THE LEGAL MOVES FOR THIS TURN
        MoveList move_list;
        generate_moves(&move_list);

        // Check for checks
        int current_king = get_lsb_index(side == white ? bitboards[K] : bitboards[k]);
        if (is_square_attacked(current_king, side == white ? black : white)) {
            cout << "\n[!] YOUR KING IS IN CHECK!\n";
        }

        string source_str, target_str;
        cout << "\nEnter source square (e.g. g1): ";
        cin >> source_str;
        cout << "Enter target square (e.g. f3): ";
        cin >> target_str;

        int source = parse_square(source_str);
        int target = parse_square(target_str);

        // --- SMART VALIDATION & PROMOTION ---
        bool move_exists = false;
        int move_to_make = 0;
        bool needs_promotion = false;

        // 1. Check if the move is legal, AND ask the engine if it requires a promotion
        for (int i = 0; i < move_list.count; i++) {
            int move = move_list.moves[i];
            if (GET_MOVE_SOURCE(move) == source && GET_MOVE_TARGET(move) == target) {
                if (GET_MOVE_PROMOTED(move) != 0) {
                    needs_promotion = true;
                }
                move_exists = true;
            }
        }

        // If the move isn't even in the list, reject it immediately before asking questions!
        if (!move_exists) {
            cout << "\n>>> ILLEGAL MOVE! (Piece cannot move there, blocked, or friendly fire). Try again.\n";
            continue;
        }

        // 2. If it IS a legal promotion, ASK the user which piece they want
        if (needs_promotion) {
            char p_char;
            cout << "Promote to (q, r, b, n): ";
            cin >> p_char;

            int user_promo_piece = 0;
            if (side == white) {
                if (p_char == 'q') user_promo_piece = Q;
                else if (p_char == 'r') user_promo_piece = R;
                else if (p_char == 'b') user_promo_piece = B;
                else if (p_char == 'n') user_promo_piece = N;
            } else {
                if (p_char == 'q') user_promo_piece = q;
                else if (p_char == 'r') user_promo_piece = r;
                else if (p_char == 'b') user_promo_piece = b;
                else if (p_char == 'n') user_promo_piece = n;
            }

            // Find the specific move that matches their chosen piece
            move_exists = false;
            for (int i = 0; i < move_list.count; i++) {
                int move = move_list.moves[i];
                if (GET_MOVE_SOURCE(move) == source && GET_MOVE_TARGET(move) == target && GET_MOVE_PROMOTED(move) == user_promo_piece) {
                    move_to_make = move;
                    move_exists = true;
                    break;
                }
            }

            if (!move_exists) {
                cout << "\n>>> INVALID PROMOTION CHARACTER! Try again.\n";
                continue;
            }
        } else {
            // 3. Normal move, just grab it
            for (int i = 0; i < move_list.count; i++) {
                int move = move_list.moves[i];
                if (GET_MOVE_SOURCE(move) == source && GET_MOVE_TARGET(move) == target) {
                    move_to_make = move;
                    break;
                }
            }
        }

        // --- FINALLY, MAKE THE MOVE ---
        if (make_move(move_to_make)) {
            cout << "\n>>> Move accepted.\n";
        } else {
            cout << "\n>>> ILLEGAL MOVE! (Leaves King in check). Try again.\n";
        }
    }
    return 0;
}