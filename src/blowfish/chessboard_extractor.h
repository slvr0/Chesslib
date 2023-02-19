#pragma once
#include <vector>
#include <random>
#include <iostream> // remove later

#include "defs.h"
#include "chessboard.h"

/*
Brief : 

Intended to provide static functions to extract detailed position information from board

*/

//takes in a square (0-63 and a board and outputs what piece is on that position)
char AtBoardPosition(const Square& square, const Board& brd);
int  AtBoardEnumerated(const Square& square, const Board& brd);
//getting the fen is computational heavy, so never use this as part of main performance heavy code, 
//if we need unique identification implement a zobrist hash function
inline std::string  BoardAsFen(const Board & board) {
    std::string res = "";
    //fen board index logic
    //56 - 63
    //48 - 55
    //40 - 47
    //32 - 39 
    //24 - 31
    //16 - 23
    //8  - 15
    //0  - 7
    
    //board content

    for(int i = 7; i >= 0 ; --i) {
        int voidsum = 0;
        for(int k = 0; k < 8; ++k){
            int index = i * 8 + k;
            char fieldchar = AtBoardPosition(1ULL << index, board);
            if(fieldchar == '.') voidsum++;
            else {
                if(voidsum) res += std::to_string(voidsum);
                res += fieldchar;
                voidsum = 0;
            }
        }
        if(voidsum) res += std::to_string(voidsum);
        if(i > 0) res += "/";
    }

    //meta data { //act, w00 / w000 /b00 / b000 , enp , half m full m }
    res += board.white_acts_ ? " w " : " b ";
    if(board.white_oo_)     res += "K";
    if(board.white_ooo_)    res += "Q";
    if(board.black_oo_)     res += "k";
    if(board.black_ooo_)    res += "q";

    if(board.white_oo_ || board.white_ooo_ || board.black_oo_ || board.black_ooo_) res += " ";
    else res += "- ";

    res += board.enp_ != -1 ? StringToLowerCase(notations[board.enp_]) : "-";
    res += " " + std::to_string(board.half_move_) + " " + std::to_string(board.full_move_); 

    return res;
}

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


FORCEINL void GenerateRandomZobristTables(unsigned long long int keys[][12]) {
    std::mt19937 mt(01234567);
    std::uniform_int_distribution<unsigned long long int>
                                 dist(0, UINT64_MAX);

    for(int i = 0 ; i < 64; ++i) {
        for(int k = 0 ; k < 12 ; ++k) {
            keys[i][k] = dist(mt);
        }
    }
}

struct ZHash {

private:
        ZHash()  {
        GenerateRandomZobristTables(keys_);
    }
    unsigned long long int keys_ [64][12];

public:
    static ZHash& GetInstance()
    {
        static ZHash    instance;                                
        return instance;
    }

    ZHash(ZHash const&)             = delete;
    void operator=(ZHash const&)    = delete;

    void GenerateNewZobristKey(Board& board) const;

    //manually xors with certain hash key, up to user to determine which switches should be done
    void UpdateZobristKey(Board& board, const int& piecetype, const int& at) const;

    static ZHash* singleton_;
};

//helps to visualize the board graphically in console
class BoardConsoleGUI {
public : 

    static void PrintBoard(const Board & board, const int & print_mode= 0, char spec_type= ' ');
    static void PrintBoard(unsigned long const & board);

    static std::string BoardAsString(const Board& board);

private:
    static std::vector<char> CopyBoardContent(const Board & board);    
};








