#pragma once
#include <vector>

#include "defs.h"
#include "chessboard.h"

/*
Brief : 

Intended to provide static functions to extract detailed position information from board

*/

//takes in a square (0-63 and a board and outputs what piece is on that position)
char AtBoardPosition(const Square& square, const Board& brd);

//prints two boards side by side. argument 1 and 2 puts additionally masks if you want to see specific things only
std::string PrintBoardsAndMask(uint64_t val1, uint64_t val2, const Board& val3, const Board& val4);

//called by black in order to know if a move steps on interferes its own pieces
FORCEINL BBoard GetWhiteOrVoid(const Board& board) {
    return ~board.black_;
}

//called by white in order to know if a move steps on interferes its own pieces
FORCEINL BBoard GetBlackOrVoid(const Board& board) {
    return ~board.white_;
}

//helps to visualize the board graphically in console
class BoardConsoleGUI {
public : 

    static void PrintBoard(const Board & board, const int & print_mode= 0, char spec_type= ' ');
    static void PrintBoard(unsigned long const & board);

private:
    static std::vector<char> CopyBoardContent(const Board & board);    
};







