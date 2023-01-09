#pragma once

#include "chessboard.h"
#include "position_meta_data.h"


FORCEINL Board UpdatePawnMove(const Board & board, const int & from, const int & to) {
    Board nb = board;

    const bool is_white = board.white_acts_;
    BBoard move = (1ULL << from | 1ULL << to);

    if(is_white)    {nb.white_pawn_      ^= move; nb.white_ ^= move;}
    else            {nb.black_pawn_      ^= move; nb.black_ ^= move;}

    nb.occ_ = nb.white_ | nb.black_;

    return nb;
}

FORCEINL Board UpdatePawnPush(const Board & board, const int & from, const int & to) {
    Board nb = board;

    const bool is_white = board.white_acts_;
    BBoard move = (1ULL << from | 1ULL << to);

    if(is_white)    {nb.white_pawn_ ^= move; nb.white_ ^= move;}
    else            {nb.black_pawn_ ^= move; nb.black_ ^= move;}

    nb.occ_ = nb.white_ | nb.black_;

    nb.enp_ = to;

    return nb;
}

FORCEINL Board UpdatePawnCapture(const Board & board, const int & from, const int & to) {
    Board nb = board;

    const bool is_white = board.white_acts_;
    const BBoard move = (1ULL << from | 1ULL << to);
    const BBoard rem = ~to;

    if(is_white)    {nb.white_pawn_ ^= move;  nb.white_ ^= move; nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;}
    else            {nb.black_pawn_ ^= move;  nb.black_ ^= move; nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;}

    nb.occ_ = nb.white_ | nb.black_;

    return nb;
}

FORCEINL Board UpdatePawnPromotion(const Board & board, const PieceType& ptype, const int & from, const int & to) {
    Board nb = board;

    const bool is_white = board.white_acts_;
    const bool is_capture = to & board.occ_;

    const BBoard move = (1ULL << from | 1ULL << to);
    const BBoard rem = ~to;
    
    if(is_white) {
        if(ptype == PieceType::KNIGHT) {nb.white_pawn_ ^= (1ULL << from ); nb.white_knight_ ^= (1ULL << to);}
        if(ptype == PieceType::BISHOP) {nb.white_pawn_ ^= (1ULL << from ); nb.white_bishop_ ^= (1ULL << to);}
        if(ptype == PieceType::ROOK)   {nb.white_pawn_ ^= (1ULL << from ); nb.white_rook_   ^= (1ULL << to);}
        if(ptype == PieceType::QUEEN)  {nb.white_pawn_ ^= (1ULL << from ); nb.white_queen_  ^= (1ULL << to);} 

        nb.white_ ^= move;

        if(is_capture) nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;      
    }      
    else {
        if(ptype == PieceType::KNIGHT) {nb.black_pawn_ ^ (1ULL << from ); nb.black_knight_ ^ (1ULL << to);}
        if(ptype == PieceType::BISHOP) {nb.black_pawn_ ^ (1ULL << from ); nb.black_bishop_ ^ (1ULL << to);}
        if(ptype == PieceType::ROOK)   {nb.black_pawn_ ^ (1ULL << from ); nb.black_rook_   ^ (1ULL << to);}
        if(ptype == PieceType::QUEEN)  {nb.black_pawn_ ^ (1ULL << from ); nb.black_queen_  ^ (1ULL << to);}

        nb.black_ ^= move;

        if(is_capture) nb.white_ &= rem; nb.white_pawn_ &=rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;
    }

    nb.occ_ = nb.white_ | nb.black_;

    return nb;
}

FORCEINL Board UpdatePawnEnpassaint(const Board & board, const int & from, const int & to, const int & enemy) {
    Board nb = board;

    const bool is_white = board.white_acts_;
    const BBoard move = (1ULL << from | 1ULL << to);
    const BBoard rem = ~(to | enemy);

    if(is_white)    {nb.white_pawn_ ^= move;  nb.white_ ^= move; nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;}
    else            {nb.black_pawn_ ^= move;  nb.black_ ^= move; nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;}
    
    nb.occ_ = nb.white_ | nb.black_;

    return nb;
}

FORCEINL Board UpdateKnightMove(const Board & board, const int & from, const int & to) {
    Board nb = board;

    const bool is_white = board.white_acts_;
    const bool is_capture = to & board.occ_;

    const BBoard move = (1ULL << from | 1ULL << to);
    const BBoard rem = ~to; 

    if(is_white) {
        nb.white_knight_ ^= move;
        nb.white_ ^= move; 

        if(is_capture) {nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;}
    }
    else {
        nb.black_knight_ ^= move;
        nb.black_ ^= move;

        if(is_capture) {nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;}
    }

    nb.occ_ = nb.white_ | nb.black_;  

    return nb; 
}

FORCEINL Board UpdateBishopMove(const Board & board, const int & from, const int & to) {
    Board nb = board;

    const bool is_white = board.white_acts_;
    const bool is_capture = to & board.occ_;

    const BBoard move = (1ULL << from | 1ULL << to);
    const BBoard rem = ~to; 

    if(is_white) {
        nb.white_bishop_ ^= move;
        nb.white_ ^= move; 

        if(is_capture) {nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;}
    }
    else {
        nb.black_bishop_ ^= move;
        nb.black_ ^= move;

        if(is_capture) {nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;}
    }

    nb.occ_ = nb.white_ | nb.black_;  

    return nb; 
}

FORCEINL Board UpdateRookMove(const Board & board, const int & from, const int & to) {
    Board nb = board;

    const bool is_white = board.white_acts_;
    const bool is_capture = to & board.occ_;

    const BBoard move = (1ULL << from | 1ULL << to);
    const BBoard rem = ~to; 

    if(is_white) {
        nb.white_rook_ ^= move;
        nb.white_ ^= move; 

        if(is_capture) {nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;}
    }
    else {
        nb.black_rook_ ^= move;
        nb.black_ ^= move;

        if(is_capture) {nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;}
    }

    nb.occ_ = nb.white_ | nb.black_;  

    return nb; 
}

FORCEINL Board UpdateQueenMove(const Board & board, const int & from, const int & to) {
    Board nb = board;

    const bool is_white = board.white_acts_;
    const bool is_capture = to & board.occ_;

    const BBoard move = (1ULL << from | 1ULL << to);
    const BBoard rem = ~to; 

    if(is_white) {
        nb.white_queen_ ^= move;
        nb.white_ ^= move; 

        if(is_capture) {nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;}
    }
    else {
        nb.black_queen_ ^= move;
        nb.black_ ^= move;

        if(is_capture) {nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;}
    }

    nb.occ_ = nb.white_ | nb.black_;  

    return nb; 
}

FORCEINL Board UpdateKingMove(const Board & board, const int & from, const int & to) {
    Board nb = board;

    const bool is_white = board.white_acts_;
    const bool is_capture = to & board.occ_;

    const BBoard move = (1ULL << from | 1ULL << to);
    const BBoard rem = ~to; 

    if(is_white) {
        nb.white_king_ ^= move;
        nb.white_ ^= move; 

        if(is_capture) {nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;}

        nb.white_oo_ = false;
        nb.white_ooo_ = false;
    }
    else {
        nb.black_king_ ^= move;
        nb.black_ ^= move;

        if(is_capture) {nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;}

        nb.black_oo_ = false;
        nb.black_ooo_ = false;
    }

    nb.occ_ = nb.white_ | nb.black_;  

    

    return nb; 
}

FORCEINL Board UpdateCastle00(const Board & board) {
    Board nb = board;

    const bool is_white = board.white_acts_;

    if(is_white) {
        nb.white_king_ ^= w00_KingMoves;
        nb.white_rook_ ^= w00_Rookmoves;

        nb.white_ ^= w00_KingMoves;
        nb.white_ ^= w00_Rookmoves;

        nb.white_oo_    = false;
        nb.white_ooo_   = false;
    }
    else {
        nb.black_king_ ^= b00_KingMoves;
        nb.black_rook_ ^= b00_Rookmoves;

        nb.black_ ^= b00_KingMoves;
        nb.black_ ^= b00_Rookmoves;

        nb.black_oo_    = false;
        nb.black_ooo_   = false;
    }

    nb.occ_ = nb.white_ | nb.black_;  

    return nb; 
}

FORCEINL Board UpdateCastle000(const Board & board) {
    Board nb = board;

    const bool is_white = board.white_acts_;
    
    if(is_white) {
        nb.white_king_^= w00_KingMoves;
        nb.white_rook_^= w000_Rookmoves;

        nb.white_ ^= w000_KingMoves;
        nb.white_ ^= w000_Rookmoves;

        nb.white_oo_    = false;
        nb.white_ooo_   = false;
    }
    else {
        nb.black_king_^= b000_KingMoves;
        nb.black_rook_^= b000_Rookmoves;

        nb.black_ ^= b000_KingMoves;
        nb.black_ ^= b000_Rookmoves;

        nb.black_oo_    = false;
        nb.black_ooo_   = false;
    }

    nb.occ_ = nb.white_ | nb.black_;  

    return nb; 
}

