#include "chess_board.h"

using namespace Chesslib;

PieceType Board::translatePieceType(char c) {
    switch(c) {
        case 'p' :
        case 'P' : return PieceType::PAWN;

        case 'b' :
        case 'B' : return PieceType::BISHOP;

        case 'n' :
        case 'N' : return PieceType::KNIGHT;

        case 'r' :
        case 'R' : return PieceType::ROOK;

        case 'q' :
        case 'Q' : return PieceType::QUEEN;

        case 'k' :
        case 'K' : return PieceType::KING;

        default : return PieceType::INVALID;
    }
}

bool Board::setFromFen(std::string fen) {
    int spos = 0;
    for(int i = 63; i>=0;) {
        char c =  fen.at(spos++);       
 
        if(isdigit(c)) { 
            int stepcount = static_cast<int> (c - 48);
            i -= stepcount;
        }
        else if(c == '/') {
            continue; // we literally dont care
        }
        else {
            PieceType type = translatePieceType(c);
            insertPiece(i--, type, isupper(c));
        }        
    }

    //read space, then who is acting 
    ++spos;
    char to_act = fen.at(spos++);
    /* std::cout << to_act; */
    state_.white_acts = to_act == 'w' ? true : false;

    //read space, then castling status
    ++spos;    

    auto update_castling = [&] (char w) {
        if(w == 'K') state_.castle |= 1ULL << 3;
        else if(w == 'Q') state_.castle |= 1ULL << 2;
        else if(w == 'k') state_.castle |= 1ULL << 1;
        else if(w == 'q') state_.castle |= 1ULL << 0;
    };

    for(int i = 0 ; i < 4 ; ++i) { 
        if(fen.at(spos) == ' ' || fen.at(spos) == '-') {
            break; 
        }
        update_castling(fen.at(spos++));
    }

    //read space, then enpassant
    ++spos;

    if(fen.at(spos) != '-') {
        state_.enp = fen.substr(spos, 2);        
        
        spos += 3;
    }
    else spos += 2;

    //read nr of moves without captures (for some 50 rule no repeat context)
    //can be 1 or 2 digits

    state_.rule50 = fen.at(spos + 1) == ' ' ? 
        static_cast<int> (fen.at(spos) - 48) :
        std::stoi(fen.substr(spos, 2));

    //traverse two or three fields depending on digits
    spos += fen.at(spos + 1) == ' ' ? 2 : 3;

    //read nr of total moves
    //can be 1 or 2 digits (3?), just read to eof

    state_.total_moves = std::stoi(fen.substr(spos, fen.size())); 
}

void Board::setHashValue() {

}

void Board::applyMove(const std::string uci_move) {
    //we always output exact information, ie. Nb1 -> Nc3 as Nb1c3 . 

    


}

void Board::applyMove(const int nn_move_index) {

}


void Board::insertPiece(const int & idx, Chesslib::PieceType type, bool white) {
    switch(type) {
        case Chesslib::PAWN :   pieces_.pawns |= 1ULL << idx; break;
        case Chesslib::BISHOP : pieces_.bishops_queens |= 1ULL << idx; break;
        case Chesslib::KNIGHT : pieces_.knights |= 1ULL << idx; break;
        case Chesslib::ROOK :   pieces_.rooks_queens |= 1ULL << idx; break;
        case Chesslib::QUEEN :  pieces_.bishops_queens |= 1ULL << idx;  pieces_.rooks_queens |= 1ULL << idx; break;
        case Chesslib::KING :   pieces_.kings |= 1ULL << idx; break;       
    }
   
    if(white) white_pieces_ |= 1ULL << idx;
    else black_pieces_ |= 1ULL << idx;
}

void Board::removePiece(const int & idx, Chesslib::PieceType type, bool white) {
        switch(type) {
        case Chesslib::PAWN :   pieces_.pawns &~ 1ULL << idx; break;
        case Chesslib::BISHOP : pieces_.bishops_queens &~1ULL << idx; break;
        case Chesslib::KNIGHT : pieces_.knights &~ 1ULL << idx; break;
        case Chesslib::ROOK :   pieces_.rooks_queens &~ 1ULL << idx; break;
        case Chesslib::QUEEN :  pieces_.bishops_queens &~ 1ULL << idx;  pieces_.rooks_queens &~ 1ULL << idx; break;
        case Chesslib::KING :   pieces_.kings &~1ULL << idx; break;        
    }
    
    if(white) white_pieces_ &~ 1ULL << idx;
    else black_pieces_ &~ 1ULL << idx;
}

void Board::updateCastleStatus(const int & new_castle_status) {

}
