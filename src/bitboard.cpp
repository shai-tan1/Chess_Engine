#include <bits/stdc++.h>
#include "bitboard.h"
using namespace std;

void print_bitboard(U64 bitboard) {
    cout << "\n";
    // 0 is white all the way upto 7 is black
    // start printing from the top row
    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col <= 7; col++) {
            int square = row * 8 + col;
            // each swuare is marked as
            // 56 57 .... 63
            //  .. .
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
}

U64 mask_knight_attacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    // both the board and attacks are 0 initially
    SET_BIT(bitboard, square);
    // put the knight on the given square

    // from the current index we move in all the possible directions
    attacks |= (bitboard >> 17) & not_h_file;  // Down 2, Left 1--> 2 * 8 + 1
    attacks |= (bitboard >> 15) & not_a_file;  // Down 2, Right 1--> 2 * 8 - 1
    attacks |= (bitboard >> 10) & not_gh_file; // Down 1, Left 2--> 1 * 8 + 2
    attacks |= (bitboard >> 6)  & not_ab_file; // Down 1, Right 2--> 1 * 8 - 2
    attacks |= (bitboard << 17) & not_a_file;  // Up 2, Right 1
    attacks |= (bitboard << 15) & not_h_file;  // Up 2, Left 1
    attacks |= (bitboard << 10) & not_ab_file; // Up 1, Right 2
    attacks |= (bitboard << 6)  & not_gh_file; // Up 1, Left 2
    // since we are alwayss going down the values decrease as the row no decreases
    // thr left cols are closer and right are farther
    // so closer meant we subtract, farther meant add
    // also check if the attacks are valid oe not
    return attacks;
}

U64 mask_king_attacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    // both the board and attacks are 0 initially
    SET_BIT(bitboard, square);

    // from the current index we move in all the possible directions
    attacks |= (bitboard << 1) & not_a_file;  // Right
    attacks |= (bitboard << 9) & not_a_file;  // Up-Right
    attacks |= (bitboard >> 7) & not_a_file;  // Down-Right
    attacks |= (bitboard >> 1) & not_h_file;  // Left
    attacks |= (bitboard << 7) & not_h_file;  // Up-Left
    attacks |= (bitboard >> 9) & not_h_file;  // Down-Left
    attacks |= (bitboard << 8); // Up
    attacks |= (bitboard >> 8); // Down
    // for up down we either go negative or we go after 63 so it is eventually destroyed
    return attacks;
}

U64 mask_pawn_attacks(int side, int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    // both the board and attacks are 0 initially
    SET_BIT(bitboard, square);

    if (side == white) {
        attacks |= (bitboard << 7) & not_a_file; // Attack Up-Left
        attacks |= (bitboard << 9) & not_h_file; // Attack Up-Right
    }
    else {
        attacks |= (bitboard >> 7) & not_h_file; // Attack Down-Right
        attacks |= (bitboard >> 9) & not_a_file; // Attack Down-Left
    }
    return attacks;
}

U64 mask_rook_attacks(int square, U64 occupancy) {
    U64 attacks = 0ULL;

    int row = square / 8;
    int col = square % 8;
    // UP
    for (int r = row + 1; r <= 7; r++) {
        attacks |= (1ULL << (r * 8 + col));
        if (occupancy & (1ULL << (r * 8 + col))) break;
    }
    // DOWN
    for (int r = row - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + col));
        if (occupancy & (1ULL << (r * 8 + col))) break;
    }
    // RIGHT
    for (int f = col + 1; f <= 7; f++) {
        attacks |= (1ULL << (row * 8 + f));
        if (occupancy & (1ULL << (row * 8 + f))) break;
    }
    // LEFT
    for (int f = col - 1; f >= 0; f--) {
        attacks |= (1ULL << (row * 8 + f));
        if (occupancy & (1ULL << (row * 8 + f))) break;
    }

    return attacks;
}
U64 mask_bishop_attacks(int square, U64 occupancy) {
    U64 attacks = 0ULL;
    int r, f;
    int row = square / 8;
    int col = square % 8;

    // UP-RIGHT
    for (r = row + 1, f = col + 1; r <= 7 && f <= 7; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    // UP-LEFT
    for (r = row + 1, f = col - 1; r <= 7 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    // DOWN-RIGHT
    for (r = row - 1, f = col + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    // DOWN-LEFT
    for (r = row - 1, f = col - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    return attacks;
}

U64 mask_queen_attacks(int square, U64 occupancy) {
    return mask_rook_attacks(square, occupancy) | mask_bishop_attacks(square, occupancy);
}