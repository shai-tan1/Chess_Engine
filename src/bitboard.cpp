#include "bitboard.h"
#include <bits/stdc++.h>
using namespace std;

U64 bitboards[12];
U64 occupancies[3];
int side = 0;
int enpassant = -1; // -1 means no en passant square available
int castle = 0;

U64 bitboards_copy[12];
U64 occupancies_copy[3];
int side_copy, enpassant_copy, castle_copy;

int count_bits(U64 bitboard) {
    int count = 0;
    while (bitboard > 0) {
        count++;
        bitboard &= (bitboard - 1);
    }
    return count;
}

int get_lsb_index(const U64 bitboard) {
    if (bitboard == 0) return -1;
    return __builtin_ctzll(bitboard);
}

bool is_square_attacked(int square, int side) {
    // 1. Attacked by enemy Pawns? (We use the INVERSE color to look backwards)
    if ((side == white) && (mask_pawn_attacks(black, square) & bitboards[P])) return true;
    if ((side == black) && (mask_pawn_attacks(white, square) & bitboards[p])) return true;

    // Knights
    if (mask_knight_attacks(square) & (side == white ? bitboards[N] : bitboards[n])) return true;

    // Kings
    if (mask_king_attacks(square) & (side == white ? bitboards[K] : bitboards[k])) return true;

    // Bishop / Queens
    if (mask_bishop_attacks(square, occupancies[2]) & (side == white ? (bitboards[B] | bitboards[Q]) : (bitboards[b] | bitboards[q]))) return true;

    // Rooks OR Queens
    if (mask_rook_attacks(square, occupancies[2]) & (side == white ? (bitboards[R] | bitboards[Q]) : (bitboards[r] | bitboards[q]))) return true;

    // If we survive all of that, the square is safe!
    return false;
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
    // since we are always going down the values decrease as the row no decreases
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
    SET_BIT(bitboard, square);

    if (side == white) {
        // Attack Up-Left (If it wraps, it goes to H. So we block H)
        attacks |= (bitboard << 7) & not_h_file;
        // Attack Up-Right (If it wraps, it goes to A. So we block A)
        attacks |= (bitboard << 9) & not_a_file;
    }
    else {
        // Attack Down-Right (If it wraps, it goes to A. So we block A)
        attacks |= (bitboard >> 7) & not_a_file;
        // Attack Down-Left (If it wraps, it goes to H. So we block H)
        attacks |= (bitboard >> 9) & not_h_file;
    }
    return attacks;
}

U64 mask_rook_attacks(int square, U64 occupancy) {
    U64 attacks = 0ULL;

    const int row = square / 8;
    const int col = square % 8;
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
    const int row = square / 8;
    const int col = square % 8;

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

bool make_move(const int move) {
    const int source = GET_MOVE_SOURCE(move);
    const int target = GET_MOVE_TARGET(move);
    const int piece_to_move = GET_MOVE_PIECE(move);
    const int is_ep = GET_MOVE_ENPASSANT(move);
    const int is_castle = GET_MOVE_CASTLING(move);
    // 1. SAVE THE STATE
    memcpy(bitboards_copy, bitboards, 96);
    memcpy(occupancies_copy, occupancies, 24);
    side_copy = side; enpassant_copy = enpassant; castle_copy = castle;

    // 2. EN PASSANT CAPTURES (Delete the pawn behind the target)
    if (is_ep) {
        if (side == white) POP_BIT(bitboards[p], target - 8);
        else POP_BIT(bitboards[P], target + 8);
    }

    // 3. NORMAL CAPTURES
    for (int i = 0; i < 12; i++) {
        if (GET_BIT(bitboards[i], target)) {
            POP_BIT(bitboards[i], target);
            break;
        }
    }

    // 4. MOVE THE PIECE
    POP_BIT(bitboards[piece_to_move], source);
    SET_BIT(bitboards[piece_to_move], target);

    if (const int promoted = GET_MOVE_PROMOTED(move)) {
        // Delete the pawn that just landed on the last rank
        POP_BIT(bitboards[piece_to_move], target);
        // Replace it with the new piece! (e.g., Queen)
        SET_BIT(bitboards[promoted], target);
    }

    // 5. MOVE THE ROOK IF CASTLING
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

    // 7. LEGALITY CHECK (King Safety)
    if (const int king_square = get_lsb_index(side == white ? bitboards[K] : bitboards[k]); is_square_attacked(king_square, side == white ? black : white)) {
        memcpy(bitboards, bitboards_copy, 96);
        memcpy(occupancies, occupancies_copy, 24);
        side = side_copy; enpassant = enpassant_copy; castle = castle_copy;
        return false;
    }

    // 8. UPDATE STATE FOR NEXT TURN
    if (piece_to_move == P && target == source + 16) enpassant = source + 8;
    else if (piece_to_move == p && target == source - 16) enpassant = source - 8;
    else enpassant = -1;

    // 9. UPDATE CASTLING RIGHTS
    // If a King moves, strip both of its castling rights.
    if (piece_to_move == K) castle &= ~(wss | wl);
    if (piece_to_move == k) castle &= ~(bs | bl);

    // If a Rook moves (or is captured!), strip that specific side's castling right.
    if (source == h1 || target == h1) castle &= ~wss;
    if (source == a1 || target == a1) castle &= ~wl;
    if (source == h8 || target == h8) castle &= ~bs;
    if (source == a8 || target == a8) castle &= ~bl;
    // (Note: To be fully complete, enpassant and castle rights need to update here based on the move,
    // but this is enough to test the physics!)

    // 10. CHANGE TURNS
    side = (side == white) ? black : white;
    return true;
}