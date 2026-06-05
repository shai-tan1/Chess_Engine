cmake_minimum_required(VERSION 4.2)
project(MyChessEngine)

set(CMAKE_CXX_STANDARD 20)
add_executable(MyChessEngine
        src/main.cpp
        src/types.h
        src/bitboard.cpp
        src/bitboard.h
        src/board.h
        src/board.cpp
        src/move.cpp
        src/move.h
        src/movegen.cpp
        src/evaluate.cpp
        src/evaluate.h
        src/search.h
        src/movegen.h
        src/tt.h
        src/uci.h
        src/tt.cpp
        src/uci.cpp
        src/search.cpp
        src/perfit.h
        src/perfit.cpp
)
