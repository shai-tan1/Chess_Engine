# ♟️ C++ Bitboard Chess Engine

A custom-built, high-performance chess engine written in C++ from scratch. This project currently focuses on an ultra-fast **Bitboard** architecture for representing the board and generating pseudo-legal piece attacks.

## 🚀 Current Status: Move Generation Foundation Complete
The engine's "brain" is successfully established. It uses 64-bit integers (`U64`) to represent piece locations and utilizes advanced bitwise operations to calculate attacks instantly, bypassing slow 2D array loops.

### ✨ Features Implemented
* **Bitboard Representation:** Complete mapping of the 64-square board using 64-bit unsigned integers.
* **Core Bit Manipulations:** * Implemented `SET_BIT`, `GET_BIT`, and `POP_BIT` macros.
  * Fast bit-counting using **Brian Kernighan's Algorithm**.
  * Instant Least Significant Bit (LSB) indexing using compiler-optimized hardware instructions (`__builtin_ctzll`).
* **Leaping Piece Move Generation:** Instant attack mapping for Knights and Kings using bit-shifting and Hexadecimal File Masks (`not_a_file`, `not_h_file`, etc.) to prevent board wrap-around bugs.
* **Pawn Move Generation:** Color-aware directional shifting for accurate pawn captures.
* **Sliding Piece Ray-Casting:** Advanced loop-based ray casting for Rooks, Bishops, and Queens.
* **Occupancy Detection:** Sliding piece rays successfully detect blockers via bitwise `AND` logic, allowing pieces to correctly stop at the first encountered piece (friendly or enemy).

## 📂 Project Structure
* `types.h` - Contains the foundational enumerations for Squares (a1 to h8) and Colors.
* `bitboard.h` - Contains core bit-manipulation macros, Hexadecimal edge masks, and function prototypes.
* `bitboard.cpp` - The engine's core math logic, housing the move generation for all 6 piece types.
* `main.cpp` - The primary diagnostic testing suite for validating bitboard maps and occupancy blockers.

## 🛠️ Building and Running
This project is developed using standard C++ and can be built easily in Linux environments (e.g., Zorin OS).

**Using CMake (CLion Default):**
Simply open the project in CLion and click the green `Play` button. CMake will automatically handle the build process.

**Using Terminal (g++):**
To compile and run the engine directly from a Linux terminal:
```bash
g++ src/main.cpp src/bitboard.cpp -o engine
./engine
