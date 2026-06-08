#include "uci.h"
#include "board.h"
#include "movegen.h"
#include "search.h"
#include "bitboard.h"
#include "tt.h" // <--- The Transposition Table Header
#include <iostream>
#include <sstream>

using namespace std;

void print_move(int move) {
    if (move == 0) return;
    int source = GET_MOVE_SOURCE(move);
    int target = GET_MOVE_TARGET(move);
    string files = "abcdefgh";
    string ranks = "12345678";
    cout << files[source % 8] << ranks[source / 8] << files[target % 8] << ranks[target / 8];

    int promoted = GET_MOVE_PROMOTED(move);
    if (promoted == Q || promoted == q) cout << 'q';
    if (promoted == R || promoted == r) cout << 'r';
    if (promoted == B || promoted == b) cout << 'b';
    if (promoted == N || promoted == n) cout << 'n';
}

int parse_move(const string& move_string) {
    MoveList move_list;
    generate_moves(&move_list);

    int source = (move_string[1] - '1') * 8 + (move_string[0] - 'a');
    int target = (move_string[3] - '1') * 8 + (move_string[2] - 'a');

    for (int i = 0; i < move_list.count; i++) {
        int move = move_list.moves[i];
        if (GET_MOVE_SOURCE(move) == source && GET_MOVE_TARGET(move) == target) {
            int promoted = GET_MOVE_PROMOTED(move);
            if (promoted) {
                if (move_string.length() == 5) {
                    char promo_char = move_string[4];
                    if (promo_char == 'q' && (promoted == Q || promoted == q)) return move;
                    if (promo_char == 'r' && (promoted == R || promoted == r)) return move;
                    if (promo_char == 'b' && (promoted == B || promoted == b)) return move;
                    if (promo_char == 'n' && (promoted == N || promoted == n)) return move;
                    continue;
                }
            }
            return move;
        }
    }
    return 0;
}

void uci_loop() {
    // Identity protocol
    cout << "id name MyChessEngine v2.0\n";
    cout << "id author Shaunak Samanta\n";
    cout << "uciok\n";

    // --- TURN ON THE TT AND HASHING ---
    init_random_keys();
    init_tt(64); // Allocate 64MB of RAM for the brain!

    string line;
    while (getline(cin, line)) {
        stringstream ss(line);
        string command;
        ss >> command;

        if (command == "isready") {
            cout << "readyok\n";
        }
        else if (command == "ucinewgame") {
            parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        }
        else if (command == "position") {
            string token;
            ss >> token;
            if (token == "startpos") {
                parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                history_ply = 0; // Reset history for repetitions
                ss >> token;
            } else if (token == "fen") {
                string fen = "";
                while (ss >> token && token != "moves") {
                    fen += token + " ";
                }
                parse_fen(fen);
                history_ply = 0; // Reset history for repetitions
            }

            // Replay the game history
            while (ss >> token) {
                int move = parse_move(token);
                if (move) {
                    make_move(move);
                    game_history[history_ply++] = generate_hash_key();
                }
            }
        }
        else if (command == "go") {
            int depth = 64; // Set a massive theoretical depth. The clock will stop it!
            long long time_to_think = 5000; // Default 5 seconds if playing without a clock

            string token;
            while (ss >> token) {
                if (token == "depth") {
                    ss >> depth;
                }
                else if (token == "wtime" && side == white) {
                    long long wtime; ss >> wtime;
                    time_to_think = wtime / 30; // Allocate 1/30th of remaining time
                }
                else if (token == "btime" && side == black) {
                    long long btime; ss >> btime;
                    time_to_think = btime / 30; // Allocate 1/30th of remaining time
                }
                else if (token == "movetime") {
                    ss >> time_to_think;
                }
            }

            // Safety buffer: drop 50ms so it doesn't flag due to internet lag
            if (time_to_think > 100) time_to_think -= 50;

            // LAUNCH THE TIME-MANAGED SEARCH
            iterative_deepening(depth, time_to_think);

            cout << "bestmove ";
            print_move(best_move);
            cout << "\n";
        }
        else if (command == "quit") {
            break;
        }
    }
}