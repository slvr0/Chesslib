#include "chessboard_traverser.h"

#include "assert.h"

BoardState UpdateBoardState(const Board& board, const MoveTypes& movetype, const int & from, const int & to) {
    bool white_acts   = board.state_.white_acts_;
    bool white00      = board.state_.white_oo_;
    bool white000     = board.state_.white_ooo_;
    bool black00      = board.state_.black_oo_;
    bool black000     = board.state_.black_ooo_;
    int  enp          = -1;  

    switch(movetype) {
        case MoveTypes::PawnPush : {  enp = to; break; }
        case MoveTypes::PieceCapture : {
            if(white_acts) {
                if(to == 56) black000 = false;
                else if(to == 63) black00 = false;
            }
            else {
                if(to == 0) white000 = false;
                else if(to == 7) white00 = false;     
            }
            break; 
        }
        case MoveTypes::RookMove : {
            if(white_acts) {
                if(from == 0) white000 = false;
                else if(from == 7) white00 = false; 
            }
            else {
                if(from == 56) black000 = false;
                else if(from == 63) black00 = false;
            }
        }
        case MoveTypes::PawnCapture     : 
        case MoveTypes::KnightCapture   :
        case MoveTypes::BishopCapture   : 
        case MoveTypes::RookCapture     : 
        case MoveTypes::QueenCapture    : 
        case MoveTypes::KingMove        : {
            if(white_acts) {           
                white00 = false;
                white000 = false;                       
            }
            else {
                black00 = false;
                black000 = false;                    
            }
            break;            
        }
        case MoveTypes::Castle00 : {
            if(white_acts) white00 = false;
            else black00 = false;
            break;   
        }
        case MoveTypes::Castle000 : {
            if(white_acts) white000 = false;
            else black000 = false;
            break;   
        }

    }

    return BoardState(!white_acts, white00, white000, black00, black000, enp);
}

Board UpdateMove(const Board& board, const MoveTypes& mtype, const PieceType& ptype, const int & from, const int & to) {
    const BBoard wp = board.white_pawn_;
    const BBoard wn = board.white_knight_;
    const BBoard wb = board.white_bishop_;
    const BBoard wr = board.white_rook_;
    const BBoard wq = board.white_queen_;
    const BBoard wk = board.white_king_;

    const BBoard bp = board.black_pawn_;
    const BBoard bn = board.black_knight_;
    const BBoard bb = board.black_bishop_;
    const BBoard br = board.black_rook_;
    const BBoard bq = board.black_queen_;
    const BBoard bk = board.black_king_;

    const BBoard move = from | to;
    const BBoard rem = ~to;

    const bool is_capture = to & board.occ_;
    const bool is_white = board.state_.white_acts_;
    
    const BoardState state = UpdateBoardState(board, mtype, from, to); 
     
    if(is_capture) {
        if(is_white) {
            if(ptype == PieceType::PAWN)   {return Board(wp ^ move, wn, wb, wr, wq, wk, bp & rem, bn & rem, bb & rem, br & rem, bq & rem, bk, state);}
            if(ptype == PieceType::KNIGHT) {return Board(wp, wn ^ move, wb, wr, wq, wk, bp & rem, bn & rem, bb & rem, br & rem, bq & rem, bk, state);}
            if(ptype == PieceType::BISHOP) {return Board(wp, wn, wb ^ move, wr, wq, wk, bp & rem, bn & rem, bb & rem, br & rem, bq & rem, bk, state);}
            if(ptype == PieceType::ROOK)   {return Board(wp, wn, wb, wr ^ move, wq, wk, bp & rem, bn & rem, bb & rem, br & rem, bq & rem, bk, state);}
            if(ptype == PieceType::QUEEN)  {return Board(wp, wn, wb, wr, wq ^ move, wk, bp & rem, bn & rem, bb & rem, br & rem, bq & rem, bk, state);}
            if(ptype == PieceType::KING)   {return Board(wp, wn, wb, wr, wq, wk ^ move, bp & rem, bn & rem, bb & rem, br & rem, bq & rem, bk, state);}
        }
        else {
            if(ptype == PieceType::PAWN)   {return Board(wp & rem, wn & rem, wb & rem, wr & rem, wq & rem, wk, bp ^ move, bn, bb, br, bq, bk, state);}
            if(ptype == PieceType::KNIGHT) {return Board(wp & rem, wn & rem, wb & rem, wr & rem, wq & rem, wk, bp, bn ^ move, bb, br, bq, bk, state);}
            if(ptype == PieceType::BISHOP) {return Board(wp & rem, wn & rem, wb & rem, wr & rem, wq & rem, wk, bp, bn, bb ^ move, br, bq, bk, state);}
            if(ptype == PieceType::ROOK)   {return Board(wp & rem, wn & rem, wb & rem, wr & rem, wq & rem, wk, bp, bn, bb, br ^ move, bq, bk, state);}
            if(ptype == PieceType::QUEEN)  {return Board(wp & rem, wn & rem, wb & rem, wr & rem, wq & rem, wk, bp, bn, bb, br, bq ^ move, bk, state);}
            if(ptype == PieceType::KING)   {return Board(wp & rem, wn & rem, wb & rem, wr & rem, wq & rem, wk, bp, bn, bb, br, bq, bk ^ move, state);}
        }    }
    else {
        if(is_white) {
            if(ptype == PieceType::PAWN)   {return Board(wp ^ move, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk, state);}
            if(ptype == PieceType::KNIGHT) {return Board(wp, wn ^ move, wb, wr, wq, wk, bp, bn, bb, br, bq, bk, state);}
            if(ptype == PieceType::BISHOP) {return Board(wp, wn, wb ^ move, wr, wq, wk, bp, bn, bb, br, bq, bk, state);}
            if(ptype == PieceType::ROOK)   {return Board(wp, wn, wb, wr ^ move, wq, wk, bp, bn, bb, br, bq, bk, state);}
            if(ptype == PieceType::QUEEN)  {return Board(wp, wn, wb, wr, wq ^ move, wk, bp, bn, bb, br, bq, bk, state);}
            if(ptype == PieceType::KING)   {return Board(wp, wn, wb, wr, wq, wk ^ move, bp, bn, bb, br, bq, bk, state);}
        }
        else {
            if(ptype == PieceType::PAWN)   {return Board(wp, wn, wb, wr, wq, wk, bp ^ move, bn, bb, br, bq, bk, state);}
            if(ptype == PieceType::KNIGHT) {return Board(wp, wn, wb, wr, wq, wk, bp, bn ^ move, bb, br, bq, bk, state);}
            if(ptype == PieceType::BISHOP) {return Board(wp, wn, wb, wr, wq, wk, bp, bn, bb ^ move, br, bq, bk, state);}
            if(ptype == PieceType::ROOK)   {return Board(wp, wn, wb, wr, wq, wk, bp, bn, bb, br ^ move, bq, bk, state);}
            if(ptype == PieceType::QUEEN)  {return Board(wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq ^ move, bk, state);}
            if(ptype == PieceType::KING)   {return Board(wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk ^ move, state);}
        } 
    }

    //m_assert((bk & mov) == 0, "Taking Black King is not legal!");
    
    //cant get here
    return Board(board);
}


Board UpdateENPCapture(const Board& board, const int & from, const int & to, const int& enemy) {
    const BBoard wp = board.white_pawn_;
    const BBoard wn = board.white_knight_;
    const BBoard wb = board.white_bishop_;
    const BBoard wr = board.white_rook_;
    const BBoard wq = board.white_queen_;
    const BBoard wk = board.white_king_;

    const BBoard bp = board.black_pawn_;
    const BBoard bn = board.black_knight_;
    const BBoard bb = board.black_bishop_;
    const BBoard br = board.black_rook_;
    const BBoard bq = board.black_queen_;
    const BBoard bk = board.black_king_;

    const BBoard move = from | to;
    const BBoard rem = ~to;

    const bool is_white = board.state_.white_acts_;
    const BoardState state = UpdateBoardState(board, MoveTypes::PawnEnp, from, to);

    if(is_white) {
        return Board(wp ^ move, wn, wb, wr, wq, wk, bp ^ enemy, bn, bb, br, bq, bk, state);
    }
    else {
        return Board(wp ^ enemy, wn, wb, wr, wq, wk, bp ^ move, bn, bb, br, bq, bk, state);
    }
}


Board UpdatePromotion(const Board& board, const MoveTypes& mtype, const PieceType& ptype, const int & from, const int & to) {
    const BBoard wp = board.white_pawn_;
    const BBoard wn = board.white_knight_;
    const BBoard wb = board.white_bishop_;
    const BBoard wr = board.white_rook_;
    const BBoard wq = board.white_queen_;
    const BBoard wk = board.white_king_;

    const BBoard bp = board.black_pawn_;
    const BBoard bn = board.black_knight_;
    const BBoard bb = board.black_bishop_;
    const BBoard br = board.black_rook_;
    const BBoard bq = board.black_queen_;
    const BBoard bk = board.black_king_;

    const BBoard move = from | to;
    const BBoard rem = ~to;

    const bool is_capture = to & board.occ_;
    const bool is_white = board.state_.white_acts_;

    const BoardState state = UpdateBoardState(board, MoveTypes::PawnPromote, from, to);

    if(is_white) {
        if(ptype == PieceType::KNIGHT) {return Board(wp ^ from, wn ^ to, wb, wr, wq, wk, bp & rem, bn & rem, bb & rem, br & rem, bq & rem, bk, state);}
        if(ptype == PieceType::BISHOP) {return Board(wp ^ from, wn, wb ^ to, wr, wq, wk, bp & rem, bn & rem, bb & rem, br & rem, bq & rem, bk, state);}
        if(ptype == PieceType::ROOK)   {return Board(wp ^ from, wn, wb, wr ^ to, wq, wk, bp & rem, bn & rem, bb & rem, br & rem, bq & rem, bk, state);}
        if(ptype == PieceType::QUEEN)  {return Board(wp ^ from, wn, wb, wr, wq ^ to, wk, bp & rem, bn & rem, bb & rem, br & rem, bq & rem, bk, state);}
    }
    else {
        if(ptype == PieceType::KNIGHT) {return Board(wp & rem, wn & rem, wb & rem, wr & rem, wq & rem, wk, bp ^ from, bn ^ to, bb, br, bq, bk, state);}
        if(ptype == PieceType::BISHOP) {return Board(wp & rem, wn & rem, wb & rem, wr & rem, wq & rem, wk, bp ^ from, bn, bb ^ to, br, bq, bk, state);}
        if(ptype == PieceType::ROOK)   {return Board(wp & rem, wn & rem, wb & rem, wr & rem, wq & rem, wk, bp ^ from, bn, bb, br ^ to, bq, bk, state);}
        if(ptype == PieceType::QUEEN)  {return Board(wp & rem, wn & rem, wb & rem, wr & rem, wq & rem, wk, bp ^ from, bn, bb, br, bq ^ to, bk, state);}
    }
}

Board UpdateCastling00(const Board& board, const int & from, const int &to) {
    const BBoard wp = board.white_pawn_;
    const BBoard wn = board.white_knight_;
    const BBoard wb = board.white_bishop_;
    const BBoard wr = board.white_rook_;
    const BBoard wq = board.white_queen_;
    const BBoard wk = board.white_king_;

    const BBoard bp = board.black_pawn_;
    const BBoard bn = board.black_knight_;
    const BBoard bb = board.black_bishop_;
    const BBoard br = board.black_rook_;
    const BBoard bq = board.black_queen_;
    const BBoard bk = board.black_king_;

    const bool is_white = board.state_.white_acts_;
    const BoardState state = UpdateBoardState(board, MoveTypes::Castle00, from, to);

    if(is_white) {
        Square _00_kingfrom  = 1ULL << 4;
        Square _00_kingto    = 1ULL << 6;
        Square _00_rookfrom  = 1ULL << 7;
        Square _00_rookto    = 1ULL << 5;

        return Board(wp, wn, wb, wr ^ (_00_rookfrom | _00_rookto), wq, wk ^ (_00_kingfrom | _00_kingto), bp, bn, bb, br, bq, bk, state);
    }
    else {
        Square _00_kingfrom  = 1ULL << 60;
        Square _00_kingto    = 1ULL << 62;
        Square _00_rookfrom  = 1ULL << 63;
        Square _00_rookto    = 1ULL << 61;

        return Board(wp, wn, wb, wr, wq, wk, bp, bn, bb, br ^ (_00_rookfrom | _00_rookto), bq, bk ^ (_00_kingfrom | _00_kingto), state);
    }
}

Board UpdateCastling000(const Board& board, const int & from, const int &to) {
    const BBoard wp = board.white_pawn_;
    const BBoard wn = board.white_knight_;
    const BBoard wb = board.white_bishop_;
    const BBoard wr = board.white_rook_;
    const BBoard wq = board.white_queen_;
    const BBoard wk = board.white_king_;

    const BBoard bp = board.black_pawn_;
    const BBoard bn = board.black_knight_;
    const BBoard bb = board.black_bishop_;
    const BBoard br = board.black_rook_;
    const BBoard bq = board.black_queen_;
    const BBoard bk = board.black_king_;

    const bool is_white = board.state_.white_acts_;
    const BoardState state = UpdateBoardState(board, MoveTypes::Castle000, from, to);

    if(is_white) {
        Square _00_kingfrom  = 1ULL << 4;
        Square _00_kingto    = 1ULL << 2;
        Square _00_rookfrom  = 1ULL << 0;
        Square _00_rookto    = 1ULL << 3;

        return Board(wp, wn, wb, wr ^ (_00_rookfrom | _00_rookto), wq, wk ^ (_00_kingfrom | _00_kingto), bp, bn, bb, br, bq, bk, state);
    }
    else {
        Square _00_kingfrom  = 1ULL << 60;
        Square _00_kingto    = 1ULL << 58;
        Square _00_rookfrom  = 1ULL << 56;
        Square _00_rookto    = 1ULL << 59;

        return Board(wp, wn, wb, wr, wq, wk, bp, bn, bb, br ^ (_00_rookfrom | _00_rookto), bq, bk ^ (_00_kingfrom | _00_kingto), state);
    }
}

//here are list of special cases to acknowledge

//pawns

//if the move is enpassaint we need to remove enp square aswell
//if promotion we need to add four different moves
//if the move is push we need to add enp square to state

//knights
//no special rules?

//bishops
//no special rules?

//rook
//check if it is a castling rook being moved, if such regulate castling state

//queen
//no special 

//king
//regulate castling 

//castling
//regulate state and proper movement

//in general
//check if capturing castling rook , regulate castling





