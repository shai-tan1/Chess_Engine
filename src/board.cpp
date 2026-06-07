#include "board.h"
#include "bitboard.h"
#include <iostream>
#include <cstring>
#include <bitset>
using namespace std;

void print_board() {
    cout << "\n";

    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col <= 7; col++) {
            // White: P, N, B, R, Q, K | Black: p, n, b, r, q, k
            constexpr char pieces[12] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k'};
            const int square = row * 8 + col;

            if (col == 0) cout << row + 1 << "  ";

            int piece = -1;
            for (int i = 0; i < 12; i++) {
                if (GET_BIT(bitboards[i], square)) {
                    piece = i;
                    break;
                }
            }

            if (piece == -1) cout << ". ";
            else cout << pieces[piece] << " ";
        }
        cout << "\n";
    }
    cout << "\n   a b c d e f g h\n\n";
    cout << "Side to move: " << (side == white ? "White" : "Black") << "\n";
}

void print_bitboard(const U64 bitboard) {
    cout << "\n";
    // 0 is white all the way upto 7 is black
    // start printing from the top row
    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col <= 7; col++) {
            const int square = row * 8 + col;
            // each square is marked as
            // 56 57 .... 63
            //  . .
            // 0 1 2 ... 8
            // print the row number
            if (col == 0) cout << row + 1 << "  ";
            // then start printing whether there are any pieces in this square
            if (GET_BIT(bitboard, square)) {
                cout << "1 ";
            }
            else {
                cout << ". ";
            }
        }
        cout << "\n";
    }
    cout << "\n   a b c d e f g h\n\n";
    cout << "Bitboard Value: " << bitboard << "ULL\n";
    cout << "Binary:  " << bitset<64>(bitboard) << "\n";
}

void parse_fen(const string &fen) {
    memset(bitboards, 0ULL, sizeof(bitboards));
    memset(occupancies, 0ULL, sizeof(occupancies));
    side = white;
    enpassant = -1;
    castle = 0;

    int row = 7, col = 0, i = 0;

    while (fen[i] != ' ') {
        if (const char c = fen[i]; (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            int piece = -1;
            if (c == 'P') piece = P; else if (c == 'N') piece = N; else if (c == 'B') piece = B;
            else if (c == 'R') piece = R; else if (c == 'Q') piece = Q; else if (c == 'K') piece = K;
            else if (c == 'p') piece = p; else if (c == 'n') piece = n; else if (c == 'b') piece = b;
            else if (c == 'r') piece = r; else if (c == 'q') piece = q; else if (c == 'k') piece = k;

            SET_BIT(bitboards[piece], (row * 8 + col));
            col++;
        }
        else if (c >= '1' && c <= '8') col += (c - '0');
        else if (c == '/') { row--; col = 0; }
        i++;
    }
    i++;
    side = (fen[i] == 'w') ? white : black;
    i += 2;

    while (fen[i] != ' ') {
        if (fen[i] == 'K') castle |= wss;
        else if (fen[i] == 'Q') castle |= wl;
        else if (fen[i] == 'k') castle |= bs;
        else if (fen[i] == 'q') castle |= bl;
        else if (fen[i] == '-') {} // No castling rights
        i++;
    }
    i++;
    if (fen[i] == '-') {
        enpassant = -1;
    }
    else {
        const int ep_col = fen[i] - 'a';
        const int ep_row = fen[i + 1] - '1';
        enpassant = ep_row * 8 + ep_col;
    }
    for (int j = P; j <= K; j++) occupancies[white] |= bitboards[j];
    for (int j = p; j <= k; j++) occupancies[black] |= bitboards[j];
    occupancies[2] = occupancies[white] | occupancies[black];
}

int parse_square(const string &sq) {
    const int col = sq[0] - 'a';
    const int row = sq[1] - '1';
    return row * 8 + col;
}

// --- ZOBRIST HASHING GLOBALS ---
U64 piece_keys[12][64];
U64 enpassant_keys[64];
U64 castle_keys[16];
U64 side_key;

U64 game_history[1024];
int history_ply = 0;

// Fast random number generator
unsigned int random_state = 1804289383;
unsigned int get_random_U32() {
    unsigned int number = random_state;
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;
    random_state = number;
    return number;
}

U64 get_random_U64() {
    U64 n1 = (U64)(get_random_U32()) & 0xFFFF;
    U64 n2 = (U64)(get_random_U32()) & 0xFFFF;
    U64 n3 = (U64)(get_random_U32()) & 0xFFFF;
    U64 n4 = (U64)(get_random_U32()) & 0xFFFF;
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

void init_random_keys() {
    for (int piece = 0; piece < 12; piece++) {
        for (int square = 0; square < 64; square++) {
            piece_keys[piece][square] = get_random_U64();
        }
    }
    for (int square = 0; square < 64; square++) {
        enpassant_keys[square] = get_random_U64();
    }
    for (int i = 0; i < 16; i++) {
        castle_keys[i] = get_random_U64();
    }
    side_key = get_random_U64();
}

U64 generate_hash_key() {
    U64 final_key = 0ULL;
    U64 bitboard;

    for (int piece = 0; piece < 12; piece++) {
        bitboard = bitboards[piece];
        while (bitboard) {
            int square = get_lsb_index(bitboard);
            final_key ^= piece_keys[piece][square];
            POP_BIT(bitboard, square);
        }
    }
    if (enpassant != -1) final_key ^= enpassant_keys[enpassant];
    final_key ^= castle_keys[castle];
    if (side == black) final_key ^= side_key;

    return final_key;
}