#pragma once

#include "chessboard.h"
#include "position_meta_data.h"
#include "chessboard_extractor.h"


//assured nothing is captured
FORCEINL void UpdateZobrist(Board& newboard, int from_type, int from_sq, int to_type, int to_sq) { //to type == just in case promo
    ZHash::GetInstance().UpdateZobristKey(newboard, from_type, from_sq); //make sure this is LSquared in BBoard inputs
    //xor prev. target piece
    
    ZHash::GetInstance().UpdateZobristKey(newboard, to_type, to_sq);   
}

FORCEINL void UpdateZobrist(const Board& oldboard, Board& newboard, int from_type, int from_sq, int to_type, int to_sq) {
    int ptypetarget = AtBoardEnumerated(to_sq, oldboard); 
    if(ptypetarget != -1) {
        ZHash::GetInstance().UpdateZobristKey(newboard, ptypetarget, to_sq);
    }

    UpdateZobrist(newboard, from_type, from_sq, to_type, to_sq);  
}

//checks whether a rook on resp. castling side was captured
FORCEINL void UpdateWhiteCastleStatusCheck(Board & board, const BBoard & to, const bool& update_hash = true) {
    if(to & WRookR) board.white_oo_     = false;
    if(to & WRookL) board.white_ooo_    = false;
}

FORCEINL void UpdateBlackCastleStatusCheck(Board & board, const BBoard & to, const bool& update_hash = true) {
    if(to & BRookR) board.black_oo_     = false;
    if(to & BRookL) board.black_ooo_    = false;
}

FORCEINL Board UpdatePawnMove(const Board & board, const BBoard & from, const BBoard & to, const bool& update_hash = true) {
    Board nb = board;

    BBoard move = (from | to);

    if(board.white_acts_) {nb.white_pawn_      ^= move; nb.white_ ^= move;}
    else                  {nb.black_pawn_      ^= move; nb.black_ ^= move; ++nb.full_move_;}

    nb.occ_ = nb.white_ | nb.black_;

    nb.enp_ = -1;

    nb.white_acts_ = !nb.white_acts_;

    nb.half_move_ = 0;

    if(update_hash) {
        if(board.white_acts_)
            UpdateZobrist(nb, 0, LSquare(from), 0, LSquare(to));
        else 
            UpdateZobrist(nb, 6, LSquare(from), 6, LSquare(to));
    }

    return nb;
}

FORCEINL Board UpdatePawnPush(const Board & board, const BBoard & from, const BBoard & to, const bool& update_hash = true) {
    Board nb = board;

    BBoard move = (from | to);

    if(board.white_acts_)    {nb.white_pawn_ ^= move; nb.white_ ^= move;}
    else            {nb.black_pawn_ ^= move; nb.black_ ^= move;++nb.full_move_;}

    nb.occ_ = nb.white_ | nb.black_;   

    nb.white_acts_ = !nb.white_acts_;

    nb.enp_ = nb.white_acts_ ? LSquare(to) + 8 :  LSquare(to) - 8;

    nb.half_move_ = 0;

    if(update_hash) {
        if(board.white_acts_)
            UpdateZobrist(nb, 0, LSquare(from), 0, LSquare(to));
        else 
            UpdateZobrist(nb, 6, LSquare(from), 6, LSquare(to));
    }

    return nb;
}

FORCEINL Board UpdatePawnCapture(const Board & board, const BBoard & from, const BBoard & to, const bool& update_hash = true) {
    Board nb = board;

    const BBoard move = (from | to);
    const BBoard rem = ~to;

    if(board.white_acts_)    {
        nb.white_pawn_ ^= move;  nb.white_ ^= move; nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;
        UpdateBlackCastleStatusCheck(nb, to);
    }
    else            {
        nb.black_pawn_ ^= move;  nb.black_ ^= move; nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;
        UpdateWhiteCastleStatusCheck(nb, to);
        ++nb.full_move_;
    }

    nb.occ_ = nb.white_ | nb.black_;

    nb.enp_ = -1;

    nb.white_acts_ = !nb.white_acts_;

    nb.half_move_ = 0;

    if(update_hash) {
        if(board.white_acts_)
            UpdateZobrist(board, nb, 0, LSquare(from), 0, LSquare(to));
        else 
            UpdateZobrist(board, nb, 6, LSquare(from), 6, LSquare(to));
    }

    return nb;
}

FORCEINL Board UpdatePawnPromotion(const Board & board, const PieceType& ptype, const BBoard & from, const BBoard & to, const bool& update_hash = true) {
    Board nb = board;

    const bool is_capture = to & board.occ_;

    const BBoard move = (from | to);
    const BBoard rem = ~to;
    
    int ptype_zstyle;

    if(board.white_acts_) {
        if(ptype == PieceType::KNIGHT) {nb.white_pawn_ ^= from; nb.white_knight_ ^= to; ptype_zstyle = 1;}
        if(ptype == PieceType::BISHOP) {nb.white_pawn_ ^= from; nb.white_bishop_ ^= to; ptype_zstyle = 2;}
        if(ptype == PieceType::ROOK)   {nb.white_pawn_ ^= from; nb.white_rook_   ^= to; ptype_zstyle = 3;}
        if(ptype == PieceType::QUEEN)  {nb.white_pawn_ ^= from; nb.white_queen_  ^= to; ptype_zstyle = 4;} 

        nb.white_ ^= move;

        if(is_capture) {
            nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;      
            UpdateBlackCastleStatusCheck(nb, to); 
        }
    }      
    else {
        if(ptype == PieceType::KNIGHT) {nb.black_pawn_ ^= from; nb.black_knight_ ^= to; ptype_zstyle = 7;}
        if(ptype == PieceType::BISHOP) {nb.black_pawn_ ^= from; nb.black_bishop_ ^= to; ptype_zstyle = 8;}
        if(ptype == PieceType::ROOK)   {nb.black_pawn_ ^= from; nb.black_rook_   ^= to; ptype_zstyle = 9;}
        if(ptype == PieceType::QUEEN)  {nb.black_pawn_ ^= from; nb.black_queen_  ^= to; ptype_zstyle = 10;}

        nb.black_ ^= move;
        ++nb.full_move_;
        
        if(is_capture) {
            nb.white_ &= rem; nb.white_pawn_ &=rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;
            UpdateWhiteCastleStatusCheck(nb, to);
        }
    }

    nb.occ_ = nb.white_ | nb.black_;

    nb.enp_ = -1;

    nb.white_acts_ = !nb.white_acts_;

    nb.half_move_ = 0;
    
    if(update_hash) {
        if(is_capture) UpdateZobrist(board, nb, 0 + (1 - board.white_acts_) * 6 , LSquare(from), ptype_zstyle, LSquare(to));
        else UpdateZobrist(nb, 0 + (1 - board.white_acts_) * 6 , LSquare(from), ptype_zstyle, LSquare(to));           
    }
    return nb;
}

FORCEINL Board UpdatePawnEnpassaint(const Board & board, const uint64_t & from, const uint64_t & to, const uint64_t & enemy, const bool& update_hash = true) {
    Board nb = board;

    const BBoard move = (from | to);
    const BBoard rem = ~(to | enemy);

    if(board.white_acts_)    {
        nb.white_pawn_ ^= move;  nb.white_ ^= move; nb.black_ &= rem; nb.black_pawn_ &= rem; 
        nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;
    }
    else            {
        nb.black_pawn_ ^= move;  nb.black_ ^= move; nb.white_ &= rem; nb.white_pawn_ &= rem; 
        nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;
        ++nb.full_move_;
    }
    
    nb.occ_ = nb.white_ | nb.black_;
    
    nb.enp_ = -1;

    nb.white_acts_ = !nb.white_acts_;

    nb.half_move_ = 0;

    if(update_hash) {
        if(board.white_acts_) {
            UpdateZobrist(board, nb, 0, LSquare(from), 0, LSquare(to));
            ZHash::GetInstance().UpdateZobristKey(nb, 6, LSquare(enemy)); //switch black enp pawn
        }
        else {
            UpdateZobrist(board, nb, 6, LSquare(from), 6, LSquare(to));
            ZHash::GetInstance().UpdateZobristKey(nb, 0, LSquare(enemy));
        }
    }

    return nb;
}

FORCEINL Board UpdateKnightMove(const Board & board, const int & from, const BBoard & to, const bool& update_hash = true) {
    Board nb = board;

    const bool is_capture = to & board.occ_;

    const BBoard move = (1ULL << from | to);
    const BBoard rem = ~to; 

    if(board.white_acts_) {
        nb.white_knight_ ^= move;
        nb.white_ ^= move; 

        if(is_capture) {
            nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;
            UpdateBlackCastleStatusCheck(nb, to);
            nb.half_move_ = 0;
        }
        
    }
    else {
        nb.black_knight_ ^= move;
        nb.black_ ^= move;
        ++nb.full_move_;

        if(is_capture) {
            nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;
            UpdateWhiteCastleStatusCheck(nb, to);          
        }
    }

    nb.occ_ = nb.white_ | nb.black_;  

    nb.enp_ = -1;

    nb.white_acts_ = !nb.white_acts_;

    if(is_capture) nb.half_move_ = 0;
    else ++nb.half_move_;

    if(update_hash) {
        if(is_capture) UpdateZobrist(board, nb, 1 + (1 - board.white_acts_) * 6 , from, 1 + (1 - board.white_acts_) * 6, LSquare(to));
        else UpdateZobrist(nb, 1 + (1 - board.white_acts_) * 6 , from, 1 + (1 - board.white_acts_) * 6, LSquare(to));           
    }

    return nb; 
}

FORCEINL Board UpdateBishopMove(const Board & board, const int & from, const BBoard & to, const bool& update_hash = true) {
    Board nb = board;

    const bool is_capture = to & board.occ_;

    const BBoard move = (1ULL << from | to);
    const BBoard rem = ~to; 

    if(board.white_acts_) {
        nb.white_bishop_ ^= move;
        nb.white_ ^= move; 

        if(is_capture) {
            nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;
            UpdateBlackCastleStatusCheck(nb, to);
        }
    }
    else {
        nb.black_bishop_ ^= move;
        nb.black_ ^= move;
        ++nb.full_move_;

        if(is_capture) {
            nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;
            UpdateWhiteCastleStatusCheck(nb, to);            
        }
    }

    nb.occ_ = nb.white_ | nb.black_;

    nb.enp_ = -1; 

    nb.white_acts_ = !nb.white_acts_;

    if(is_capture) nb.half_move_ = 0;
    else ++nb.half_move_;

    if(update_hash) {
        if(is_capture) UpdateZobrist(board, nb, 2 + (1 - board.white_acts_) * 6 , from, 2 + (1 - board.white_acts_) * 6, LSquare(to));
        else UpdateZobrist(nb, 2 + (1 - board.white_acts_) * 6 , from, 2+ (1 - board.white_acts_) * 6, LSquare(to));           
    }

    return nb; 
}

FORCEINL Board UpdateRookMove(const Board & board, const int & from, const BBoard & to, const bool& update_hash = true) {
    Board nb = board;

    const bool is_capture = to & board.occ_;

    const BBoard move = (1ULL << from | to);
    const BBoard rem = ~to; 

    const uint64_t from64 = 1ULL << from;

    if(board.white_acts_) {
        nb.white_rook_ ^= move;
        nb.white_ ^= move; 

        if(from64 & WRookR) nb.white_oo_ = false;
        if(from64 & WRookL) nb.white_ooo_ = false;

        if(is_capture) {
            nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;
            UpdateBlackCastleStatusCheck(nb, to);
        }
    }
    else {
        nb.black_rook_ ^= move;
        nb.black_ ^= move;

        if(from64 & BRookR) nb.black_oo_ = false;
        if(from64 & BRookL) nb.black_ooo_ = false;

        if(is_capture) {
            nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;
            UpdateWhiteCastleStatusCheck(nb, to);            
        }
        ++nb.full_move_;
    }

    nb.occ_ = nb.white_ | nb.black_;

    nb.enp_ = -1;  

    nb.white_acts_ = !nb.white_acts_;

    if(is_capture) nb.half_move_ = 0;
    else ++nb.half_move_;

    if(update_hash) {
        if(is_capture) UpdateZobrist(board, nb, 3 + (1 - board.white_acts_) * 6 , from, 3 + (1 - board.white_acts_) * 6, LSquare(to));
        else UpdateZobrist(nb, 3 + (1 - board.white_acts_) * 6 , from, 3 + (1 - board.white_acts_) * 6, LSquare(to));           
    }

    return nb;
}

FORCEINL Board UpdateQueenMove(const Board & board, const int & from, const BBoard & to, const bool& update_hash = true) {
    Board nb = board;

    const bool is_capture = to & board.occ_;

    const BBoard move = (1ULL << from | to);
    const BBoard rem = ~to; 

    if(board.white_acts_) {
        nb.white_queen_ ^= move;
        nb.white_ ^= move; 

        if(is_capture) {
            nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;
            UpdateBlackCastleStatusCheck(nb, to);
        }
    }
    else {
        nb.black_queen_ ^= move;
        nb.black_ ^= move;

        if(is_capture) {
            nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;
            UpdateWhiteCastleStatusCheck(nb, to);
        }
        ++nb.full_move_;
    }

    nb.occ_ = nb.white_ | nb.black_;  

    nb.enp_ = -1; 

    nb.white_acts_ = !nb.white_acts_;

    if(is_capture) nb.half_move_ = 0;
    else ++nb.half_move_;

    if(update_hash) {
        if(is_capture) UpdateZobrist(board, nb, 4 + (1 - board.white_acts_) * 6 , from, 4 + (1 - board.white_acts_) * 6, LSquare(to));
        else UpdateZobrist(nb, 4 + (1 - board.white_acts_) * 6 , from, 4 + (1 - board.white_acts_) * 6, LSquare(to));           
    }

    return nb; 
}

FORCEINL Board UpdateKingMove(const Board & board, const int & from, const BBoard & to, const bool& update_hash = true) {
    Board nb = board;

    const bool is_capture = to & board.occ_;

    const BBoard move = (1ULL << from | to);
    const BBoard rem = ~to; 

    if(board.white_acts_) {
        nb.white_king_ ^= move;
        nb.white_ ^= move; 

        if(is_capture) {
            nb.black_ &= rem; nb.black_pawn_ &= rem; nb.black_knight_ &= rem; nb.black_bishop_ &= rem; nb.black_rook_ &= rem; nb.black_queen_ &= rem;
            UpdateBlackCastleStatusCheck(nb, to);
        }

        nb.white_oo_ = false;
        nb.white_ooo_ = false;
    }
    else {
        nb.black_king_ ^= move;
        nb.black_ ^= move;

        if(is_capture) {
            nb.white_ &= rem; nb.white_pawn_ &= rem; nb.white_knight_ &= rem; nb.white_bishop_ &= rem; nb.white_rook_ &= rem; nb.white_queen_ &= rem;
            UpdateWhiteCastleStatusCheck(nb, to);
        }
        
        ++nb.full_move_;

        nb.black_oo_ = false;
        nb.black_ooo_ = false;
    }

    nb.occ_ = nb.white_ | nb.black_;  

    nb.enp_ = -1; 

    nb.white_acts_ = !nb.white_acts_; 

    if(is_capture) nb.half_move_ = 0;
    else ++nb.half_move_; 

    if(update_hash) {
        if(is_capture) UpdateZobrist(board, nb, 5 + (1 - board.white_acts_) * 6 , from, 5 + (1 - board.white_acts_) * 6, LSquare(to));
        else UpdateZobrist(nb, 5 + (1 - board.white_acts_) * 6 , from, 5 + (1 - board.white_acts_) * 6, LSquare(to));           
    }


    return nb; 
}

FORCEINL Board UpdateCastle00(const Board & board, const bool& update_hash = true) {
    Board nb = board;

    if(board.white_acts_) {
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
        ++nb.full_move_;
    }

    nb.occ_ = nb.white_ | nb.black_;

    nb.enp_ = -1;  

    nb.white_acts_ = !nb.white_acts_;

    ++nb.half_move_;

    if(update_hash) {
        if(board.white_acts_) {
            UpdateZobrist(nb, 5, 4, 5, 6);
            UpdateZobrist(nb, 3, 7, 3, 5);
        }
        else {
            UpdateZobrist(nb, 11, 60, 11, 62);
            UpdateZobrist(nb, 9, 63, 9, 61);
        }
    }

    return nb; 
}

FORCEINL Board UpdateCastle000(const Board & board, const bool& update_hash = true) {
    Board nb = board;
   
    if(board.white_acts_) {
        nb.white_king_^= w000_KingMoves;
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
        ++nb.full_move_;
    }

    nb.occ_ = nb.white_ | nb.black_;  

    nb.enp_ = -1; 

    nb.white_acts_ = !nb.white_acts_;

    ++nb.half_move_;

    if(update_hash) {
        if(board.white_acts_) {
            UpdateZobrist(nb, 5, 4, 5, 2);
            UpdateZobrist(nb, 3, 0, 3, 3);
        }
        else {
            UpdateZobrist(nb, 11, 60, 11, 58);
            UpdateZobrist(nb, 9, 56, 9, 59);
        }
    }

    return nb;     
}

