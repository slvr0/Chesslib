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

FORCEINL BBoard GetActive(const Board& board) {
    return board.white_acts_ ? board.white_ : board.black_;
}

FORCEINL BBoard GetActivePawns(const Board& board) {
    return board.white_acts_ ? board.white_pawn_ : board.black_pawn_;
}

FORCEINL BBoard GetEnemyPawns(const Board& board) {
    return board.white_acts_ ? board.black_pawn_ : board.white_pawn_;
}

FORCEINL BBoard GetActiveKnights(const Board& board) {
    return board.white_acts_ ? board.white_knight_ : board.black_knight_;
}

FORCEINL BBoard GetEnemyKnights(const Board& board) {
    return board.white_acts_ ? board.black_knight_ : board.white_knight_;
}

FORCEINL BBoard GetActiveBishops(const Board& board) {
    return board.white_acts_ ? board.white_bishop_ : board.black_bishop_;
}

FORCEINL BBoard GetEnemyBishops(const Board& board) {
    return board.white_acts_ ? board.black_bishop_ : board.white_bishop_;
}

FORCEINL BBoard GetActiveRooks(const Board& board) {
    return board.white_acts_ ? board.white_rook_ : board.black_rook_;
}

FORCEINL BBoard GetEnemyRooks(const Board& board) {
    return board.white_acts_ ? board.black_rook_ : board.white_rook_;
}

FORCEINL BBoard GetActiveQueens(const Board& board) {
    return board.white_acts_ ? board.white_queen_ : board.black_queen_;
}

FORCEINL BBoard GetEnemyQueens(const Board& board) {
    return board.white_acts_ ? board.black_queen_ : board.white_queen_;
}

FORCEINL BBoard GetActiveKing(const Board& board) {
    return board.white_acts_ ? board.white_king_ : board.black_king_;
}

FORCEINL BBoard GetEnemyKing(const Board& board) {
    return board.white_acts_ ? board.black_king_ : board.white_king_;
}

FORCEINL BBoard GetActiveRookQueen(const Board& board) {
    return board.white_acts_ ? board.white_rook_ | board.white_queen_ : board.black_rook_ | board.black_queen_;
}

FORCEINL BBoard GetEnemyRookQueen(const Board& board) {
    return board.white_acts_ ? board.black_rook_ | board.black_queen_ : board.white_rook_ | board.white_queen_;
}

FORCEINL BBoard GetActiveBishopQueen(const Board& board) {
    return board.white_acts_ ? board.white_bishop_ | board.white_queen_ : board.black_bishop_ | board.black_queen_;
}

FORCEINL BBoard GetEnemyBishopQueen(const Board& board) {
    return board.white_acts_ ? board.black_bishop_ | board.black_queen_ : board.white_bishop_ | board.white_queen_;
}


//helps to visualize the board graphically in console
class BoardConsoleGUI {
public : 

    static void PrintBoard(const Board & board, const int & print_mode= 0, char spec_type= ' ');
    static void PrintBoard(unsigned long const & board);

    static std::string BoardAsString(const Board& board);

private:
    static std::vector<char> CopyBoardContent(const Board & board);    
};







