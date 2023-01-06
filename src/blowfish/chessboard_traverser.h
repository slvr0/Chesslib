#pragma once

#include "chessboard.h"

enum class MoveTypes {
    PawnMove        = 0,
    PawnPush        = 1,
    PawnCapture     = 2,
    PawnPromote     = 3,
    PawnEnp         = 4,
    PieceCapture    = 5,    
    KnightMove      = 6,
    KnightCapture   = 7,
    BishopMove      = 8, 
    BishopCapture   = 9,
    RookMove        = 10,
    RookCapture     = 11,
    QueenMove       = 12,
    QueenCapture    = 13,
    KingMove        = 14,
    Castle00        = 15,
    Castle000       = 16
    
};

BoardState  UpdateBoardState(const Board& board, const MoveTypes& movetype, const int & from, const int & to);
Board       UpdateMove(const Board& board, const MoveTypes& mtype, const PieceType& ptype, const int & from, const int & to);
Board       UpdateENPCapture(const Board& board, const int & from, const int & to, const int& enemy);
Board       UpdatePromotion(const Board& board, const MoveTypes& mtype, const PieceType& ptype, const int & from, const int & to);
Board       UpdateCastling00(const Board& board, const int & from, const int &to);
Board       UpdateCastling000(const Board& board, const int & from, const int &to);

