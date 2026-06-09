#include "bitboard.h"
#include <bits/stdc++.h>
using namespace std;

// Allocate memory for the global variables defined in bitboard.h
U64 bitboards[12];
U64 occupancies[3];
int side = 0;
int enpassant = -1;
int castle = 0;

// Castling rights update mask.
// If a piece moves from or to these squares, it modifies the castling rights.
const int castling_rights[64] = {
    13, 15, 15, 15, 12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
     7, 15, 15, 15,  3, 15, 15, 11
};

// Copies used by make_move to restore the board if a move leaves the King in check
U64 bitboards_copy[12];
U64 occupancies_copy[3];
int side_copy, enpassant_copy, castle_copy;

// Brian Kernighan's Algorithm: Counts how many 1s are in a 64-bit integer
int count_bits(U64 bitboard) {
    int count = 0;
    while (bitboard > 0) {
        count++;
        bitboard &= (bitboard - 1); // Clears the lowest set bit
    }
    return count;
}

// Finds the square index of the first piece on a bitboard (Least Significant Bit)
int get_lsb_index(const U64 bitboard) {
    if (bitboard == 0) return -1;
    return __builtin_ctzll(bitboard); // Highly optimized compiler instruction
}

// Crucial function: Checks if a specific square is under attack by a specific side
bool is_square_attacked(int square, int side) {
    // 1. Pawns: We use the INVERSE color pawn attacks to look backward from the square
    if ((side == white) && (mask_pawn_attacks(black, square) & bitboards[P])) return true;
    if ((side == black) && (mask_pawn_attacks(white, square) & bitboards[p])) return true;

    // 2. Knights
    if (mask_knight_attacks(square) & (side == white ? bitboards[N] : bitboards[n])) return true;

    // 3. Kings
    if (mask_king_attacks(square) & (side == white ? bitboards[K] : bitboards[k])) return true;

    // 4. Bishops & Queens
    if (mask_bishop_attacks(square, occupancies[2]) & (side == white ? (bitboards[B] | bitboards[Q]) : (bitboards[b] | bitboards[q]))) return true;

    // 5. Rooks & Queens
    if (mask_rook_attacks(square, occupancies[2]) & (side == white ? (bitboards[R] | bitboards[Q]) : (bitboards[r] | bitboards[q]))) return true;

    return false; // If we survive all checks, the square is safe
}

// Generates all possible target squares for a Knight on a given square
U64 mask_knight_attacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    SET_BIT(bitboard, square); // Place the knight

    // Shift the bitboard to represent Knight leaps, masking files to prevent wrapping
    attacks |= (bitboard >> 17) & not_h_file;  // Down 2, Left 1
    attacks |= (bitboard >> 15) & not_a_file;  // Down 2, Right 1
    attacks |= (bitboard >> 10) & not_gh_file; // Down 1, Left 2
    attacks |= (bitboard >> 6)  & not_ab_file; // Down 1, Right 2
    attacks |= (bitboard << 17) & not_a_file;  // Up 2, Right 1
    attacks |= (bitboard << 15) & not_h_file;  // Up 2, Left 1
    attacks |= (bitboard << 10) & not_ab_file; // Up 1, Right 2
    attacks |= (bitboard << 6)  & not_gh_file; // Up 1, Left 2
    return attacks;
}

// Generates all possible target squares for a King
U64 mask_king_attacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    SET_BIT(bitboard, square);

    attacks |= (bitboard << 1) & not_a_file;  // Right
    attacks |= (bitboard << 9) & not_a_file;  // Up-Right
    attacks |= (bitboard >> 7) & not_a_file;  // Down-Right
    attacks |= (bitboard >> 1) & not_h_file;  // Left
    attacks |= (bitboard << 7) & not_h_file;  // Up-Left
    attacks |= (bitboard >> 9) & not_h_file;  // Down-Left
    attacks |= (bitboard << 8); // Up
    attacks |= (bitboard >> 8); // Down
    return attacks;
}

// Generates diagonal pawn captures
U64 mask_pawn_attacks(int side, int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    SET_BIT(bitboard, square);

    if (side == white) {
        attacks |= (bitboard << 7) & not_h_file; // Attack Up-Left
        attacks |= (bitboard << 9) & not_a_file; // Attack Up-Right
    }
    else {
        attacks |= (bitboard >> 7) & not_a_file; // Attack Down-Right
        attacks |= (bitboard >> 9) & not_h_file; // Attack Down-Left
    }
    return attacks;
}

// Ray-casting for Rooks. Stops when it hits the 'occupancy' map.
U64 mask_rook_attacks(int square, U64 occupancy) {
    U64 attacks = 0ULL;
    const int row = square / 8;
    const int col = square % 8;

    // Slide UP
    for (int r = row + 1; r <= 7; r++) {
        attacks |= (1ULL << (r * 8 + col));
        if (occupancy & (1ULL << (r * 8 + col))) break; // Blocked!
    }
    // Slide DOWN
    for (int r = row - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + col));
        if (occupancy & (1ULL << (r * 8 + col))) break;
    }
    // Slide RIGHT
    for (int f = col + 1; f <= 7; f++) {
        attacks |= (1ULL << (row * 8 + f));
        if (occupancy & (1ULL << (row * 8 + f))) break;
    }
    // Slide LEFT
    for (int f = col - 1; f >= 0; f--) {
        attacks |= (1ULL << (row * 8 + f));
        if (occupancy & (1ULL << (row * 8 + f))) break;
    }
    return attacks;
}

// Ray-casting for Bishops
U64 mask_bishop_attacks(int square, U64 occupancy) {
    U64 attacks = 0ULL;
    int r, f;
    const int row = square / 8;
    const int col = square % 8;

    // Slide UP-RIGHT
    for (r = row + 1, f = col + 1; r <= 7 && f <= 7; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    // Slide UP-LEFT
    for (r = row + 1, f = col - 1; r <= 7 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    // Slide DOWN-RIGHT
    for (r = row - 1, f = col + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    // Slide DOWN-LEFT
    for (r = row - 1, f = col - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    return attacks;
}

// Queen is just Rook + Bishop!
U64 mask_queen_attacks(int square, U64 occupancy) {
    return mask_rook_attacks(square, occupancy) | mask_bishop_attacks(square, occupancy);
}

// The heart of the engine's physics: Executes a move on the board
bool make_move(const int move) {
    const int source = GET_MOVE_SOURCE(move);
    const int target = GET_MOVE_TARGET(move);
    const int piece_to_move = GET_MOVE_PIECE(move);
    const int is_ep = GET_MOVE_ENPASSANT(move);
    const int is_castle = GET_MOVE_CASTLING(move);

    // 1. SAVE THE STATE (In case the move is illegal and leaves the King in check)
    memcpy(bitboards_copy, bitboards, 96);
    memcpy(occupancies_copy, occupancies, 24);
    side_copy = side; enpassant_copy = enpassant; castle_copy = castle;

    // 2. EN PASSANT CAPTURES (Delete the pawn located behind the target square)
    if (is_ep) {
        if (side == white) POP_BIT(bitboards[p], target - 8);
        else POP_BIT(bitboards[P], target + 8);
    }

    // 3. NORMAL CAPTURES (Delete whatever enemy piece is on the target square)
    for (int i = 0; i < 12; i++) {
        if (GET_BIT(bitboards[i], target)) {
            POP_BIT(bitboards[i], target);
            break;
        }
    }

    // 4. MOVE THE PIECE
    POP_BIT(bitboards[piece_to_move], source);
    SET_BIT(bitboards[piece_to_move], target);

    // 4b. PROMOTIONS
    if (const int promoted = GET_MOVE_PROMOTED(move)) {
        POP_BIT(bitboards[piece_to_move], target); // Remove the pawn
        SET_BIT(bitboards[promoted], target);      // Place the Queen/Knight
    }

    // 5. CASTLING (We moved the King, now we must move the Rook)
    if (is_castle) {
        if (target == g1) { POP_BIT(bitboards[R], h1); SET_BIT(bitboards[R], f1); } // White Kingside
        else if (target == c1) { POP_BIT(bitboards[R], a1); SET_BIT(bitboards[R], d1); } // White Queenside
        else if (target == g8) { POP_BIT(bitboards[r], h8); SET_BIT(bitboards[r], f8); } // Black Kingside
        else if (target == c8) { POP_BIT(bitboards[r], a8); SET_BIT(bitboards[r], d8); } // Black Queenside
    }

    // 6. REBUILD OCCUPANCIES
    occupancies[white] = 0ULL; occupancies[black] = 0ULL;
    for (int piece = P; piece <= K; piece++) occupancies[white] |= bitboards[piece];
    for (int piece = p; piece <= k; piece++) occupancies[black] |= bitboards[piece];
    occupancies[2] = occupancies[white] | occupancies[black];

    // 7. LEGALITY CHECK (Did we just hang our own King?)
    int king_square = get_lsb_index(side == white ? bitboards[K] : bitboards[k]);
    if (is_square_attacked(king_square, side == white ? black : white)) {
        // Illegal move! Restore the board and return false.
        memcpy(bitboards, bitboards_copy, 96);
        memcpy(occupancies, occupancies_copy, 24);
        side = side_copy; enpassant = enpassant_copy; castle = castle_copy;
        return false;
    }

    // 8. UPDATE STATE FOR NEXT TURN (En Passant)
    if (piece_to_move == P && target == source + 16) enpassant = source + 8;
    else if (piece_to_move == p && target == source - 16) enpassant = source - 8;
    else enpassant = -1;

    // 9. UPDATE CASTLING RIGHTS
    castle &= castling_rights[source];
    castle &= castling_rights[target];

    // 10. CHANGE TURNS
    side = (side == white) ? black : white;
    return true; // Move was legal and successfully executed!
}