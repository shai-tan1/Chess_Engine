#pragma once
#include "types.h"

// --- MOVE ENCODING ---
// Instead of creating slow C++ Classes for a Move, we pack all information into bits.
// Bits 0-5: Source Square (63)
// Bits 6-11: Target Square (63)
// Bits 12-15: Piece (11)
// Bits 16-19: Promoted Piece (11)
// Bit 20: Capture flag
// Bit 21: Double pawn push flag
// Bit 22: En passant flag
// Bit 23: Castling flag
#define ENCODE_MOVE(source, target, piece, promoted, capture, double_push, enpassant, castling) \
((source) | \
(target << 6) | \
(piece << 12) | \
(promoted << 16) | \
(capture << 20) | \
(double_push << 21) | \
(enpassant << 22) | \
(castling << 23))

// Extract information back out of the integer using Bitwise AND (&)
#define GET_MOVE_SOURCE(move)      (move & 0x3f)          // 0x3f is 63 in hex (6 bits)
#define GET_MOVE_TARGET(move)      ((move >> 6) & 0x3f)
#define GET_MOVE_PIECE(move)       ((move >> 12) & 0xf)   // 0xf is 15 in hex (4 bits)
#define GET_MOVE_PROMOTED(move)    ((move >> 16) & 0xf)
#define GET_MOVE_CAPTURE(move)     ((move >> 20) & 0x1)
#define GET_MOVE_DOUBLE(move)      ((move >> 21) & 0x1)
#define GET_MOVE_ENPASSANT(move)   ((move >> 22) & 0x1)
#define GET_MOVE_CASTLING(move)    ((move >> 23) & 0x1)

// Struct to hold all generated moves for a specific turn
struct MoveList {
    int moves[256];  // Array of encoded move integers
    int scores[256]; // Array of heuristic scores (for move ordering)
    int count;       // How many moves are currently in the list
};

// Inline function to quickly append a move to the list
static inline void add_move(MoveList *move_list, const int move) {
    move_list->moves[move_list->count] = move;
    move_list->count++;
}