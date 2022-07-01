#include "chess_board.h"

#include <cstdlib>

#include "move_generator.h"

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
    for(int i = 56; i>=0;) {
        char c =  fen.at(spos++);  

        if(c == ' ') break;
 
        if(isdigit(c)) { 
            int stepcount = static_cast<int> (c - 48);
            i += stepcount;
        }
        else if(c == '/') {
            i -= 16;
            continue; // we literally dont care
        }        
        else {
            PieceType type = translatePieceType(c);
            insertPiece(i++, type, isupper(c));
        }        
    }

    
    //read space, then who is acting 

   /*  print(fen.substr(spos, fen.size())); */
  
    char to_act = fen.at(spos++);
    /* std::cout << to_act; */
    state_.white_acts = to_act == 'w' ? true : false;
  
    

    //read space, then castling status
    ++spos;   

    auto update_castling = [&] (char w) {
        if(w == 'K') state_.white_oo == true;
        else if(w == 'Q') state_.white_ooo == true;
        else if(w == 'k') state_.black_oo == true;
        else if(w == 'q') state_.black_ooo == true;
    };

    if(fen.at(spos) == '-') {
        ++spos;
    }
    else{
        for(int i = 0 ; i < 4 ; ++i) { 
            if(fen.at(spos) == ' ') {
                break; 
            }
            update_castling(fen.at(spos++));
        }
    }

    //read space, then enpassant
    
    
    if(fen.at(spos) != '-') {                
        std::string notation = fen.substr(spos + 1, 2);
        state_.enp = get_idx_from_notation(notation);
        
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

void Board::applyMove(const Transition& transition) {
    size_t src = transition.srcdest.first;
    size_t dst = transition.srcdest.second;
    char instr = transition.instr;
 
    //check enp and castle first then use charToPieceType for faster search

    static int captures = 0;
    if(1ULL << dst & all()) {
        print(++captures);
    }

    //set enp if eligible
    if(instr == 'p') {
        removePiece(src, PieceType::PAWN, state_.white_acts);
        removePiece(dst, pieceAt(dst).second, !state_.white_acts);
        insertPiece(dst, PieceType::PAWN, state_.white_acts);

        if((dst - src) * (dst - src) == 256) {
            
            U64 eligible_sql = (dst - 1 ) / 8 == dst / 8 ? 1ULL << (dst - 1) : 0x0;
            U64 eligible_sqr = (dst + 1 ) / 8 == dst / 8 ? 1ULL << (dst + 1) : 0x0;           

            if((eligible_sql | eligible_sqr) & filter(pieces_.pawns, !state_.white_acts)) {
                state_.enp = state_.white_acts ? dst - 8 : dst + 8;
            }
            else state_.enp = -1;
        }
    }

    else if (instr == 'b'){                    
        removePiece(src, PieceType::BISHOP, state_.white_acts);
        removePiece(dst, pieceAt(dst).second, !state_.white_acts);
        insertPiece(dst, PieceType::BISHOP, state_.white_acts);
    }

    else if (instr == 'n'){
        removePiece(src, PieceType::KNIGHT, state_.white_acts);
        removePiece(dst, pieceAt(dst).second, !state_.white_acts);
        insertPiece(dst, PieceType::KNIGHT, state_.white_acts);
    }

    else if (instr == 'r'){
        removePiece(src, PieceType::ROOK, state_.white_acts);
        removePiece(dst, pieceAt(dst).second, !state_.white_acts);
        insertPiece(dst, PieceType::ROOK, state_.white_acts);
    }

    else if (instr == 'q'){
        removePiece(src, PieceType::QUEEN, state_.white_acts);
        removePiece(dst, pieceAt(dst).second, !state_.white_acts);
        insertPiece(dst, PieceType::QUEEN, state_.white_acts);
    }
    else if (instr == 'k'){
        removePiece(src, PieceType::KING, state_.white_acts);
        removePiece(dst, pieceAt(dst).second, !state_.white_acts);
        insertPiece(dst, PieceType::KING, state_.white_acts);
    }
    else if(instr == 'O') {
        if(state_.white_acts) {
            state_.white_oo = false;
            removePiece(4, PieceType::KING, state_.white_acts);
            insertPiece(6, PieceType::KING, state_.white_acts);
            removePiece(7, PieceType::ROOK, state_.white_acts);
            insertPiece(5, PieceType::ROOK, state_.white_acts);
        }
        else {
            state_.black_oo = false;
            removePiece(60, PieceType::KING, state_.white_acts);
            insertPiece(62, PieceType::KING, state_.white_acts);
            removePiece(63, PieceType::ROOK, state_.white_acts);
            insertPiece(61, PieceType::ROOK, state_.white_acts);
        }
    } 
    else if(instr == 'o') {
        if(state_.white_acts) {
            state_.white_ooo = false;
            removePiece(4, PieceType::KING, state_.white_acts);
            insertPiece(2, PieceType::KING, state_.white_acts);
            removePiece(0, PieceType::ROOK, state_.white_acts);
            insertPiece(3, PieceType::ROOK, state_.white_acts);
        }
        else {
            state_.black_ooo = false;
            removePiece(60, PieceType::KING, state_.white_acts);
            insertPiece(58, PieceType::KING, state_.white_acts);
            removePiece(56, PieceType::ROOK, state_.white_acts);
            insertPiece(59, PieceType::ROOK, state_.white_acts);
        }
    }  
    else if(instr == 'E') {
        removePiece(src, PieceType::PAWN, state_.white_acts);
        removePiece(state_.white_acts ? dst - 8 : dst + 8, PieceType::PAWN, !state_.white_acts);
        insertPiece(dst, PieceType::PAWN, state_.white_acts);
    } 

    updateCastleStatus(src, dst, instr);
    state_.white_acts = !state_.white_acts;
    state_.total_moves += 1;    
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

    all_pieces_ |= 1ULL << idx;
}

void Board::removePiece(const int & idx, Chesslib::PieceType type, bool white) {

    if(type == PieceType::INVALID) return;

    const U64 idx_64 = 1ULL << idx;
        switch(type) {
        case Chesslib::PAWN :   pieces_.pawns &= ~idx_64; break;
        case Chesslib::BISHOP : pieces_.bishops_queens &= ~idx_64; break;
        case Chesslib::KNIGHT : pieces_.knights &= ~idx_64; break;
        case Chesslib::ROOK :   pieces_.rooks_queens &= ~idx_64; break;
        case Chesslib::QUEEN :  pieces_.bishops_queens &= ~idx_64;  pieces_.rooks_queens &= ~idx_64; break;
        case Chesslib::KING :   pieces_.kings &= ~idx_64; break;        
    }
    
    if(white) white_pieces_ &= ~idx_64;
    else black_pieces_ &= ~idx_64;

    all_pieces_ &= ~idx_64;
}

void Board::updateCastleStatus(const int & src, const int & dst, char instr) {
        if(state_.white_acts) {
        if(instr == 'k') {
            state_.white_oo = false;
            state_.white_ooo = false;
        }
        if(dst == 63) {
            state_.black_oo = false;
        }
        else if(dst == 56) {
            state_.black_ooo = false;
        }
        if(src == 7) {
            state_.white_oo = false;
        }
        else if(src == 0) {
            state_.white_ooo = false;
        }
    }
    else {
        if(instr == 'k') {
            state_.black_oo = false;
            state_.black_ooo = false;
        }
        if(dst == 7) {
            state_.white_oo = false;
        }
        else if(dst == 0) {
            state_.white_ooo = false;
        }
        if(src == 63) {
            state_.black_oo = false;
        }
        else if(src == 56) {
            state_.black_ooo = false;
        }
    } 
}

SpecificPiece Board::pieceAt(const int & idx) const {
    U64 idx_64 = 1ULL << idx; 

    auto locateType  = [&]() {
        

        if(!all_pieces_ & idx_64) return Chesslib::PieceType::INVALID;
        else if(pieces_.pawns &  idx_64) return Chesslib::PieceType::PAWN;
        else if((pieces_.bishops_queens & ~pieces_.rooks_queens) & idx_64) return Chesslib::PieceType::BISHOP;
        else if(pieces_.knights & idx_64) return Chesslib::PieceType::KNIGHT;
        else if((pieces_.rooks_queens & ~pieces_.bishops_queens) & idx_64) return Chesslib::PieceType::ROOK;
        else if((pieces_.rooks_queens & pieces_.bishops_queens) & idx_64) return Chesslib::PieceType::QUEEN;
        else if(pieces_.kings & idx_64) return Chesslib::PieceType::KING;
    };

    Chesslib::PieceType type  = locateType();
    return white_pieces_ & idx_64 ? SpecificPiece(true, type) : SpecificPiece(false, type); 
}

void BoardConsoleGUI::translateTransitions(const Chesslib::Board & board, const Transitions & transitions) {

    
    for(const auto & transition : transitions) {
        std::cout << BoardConsoleGUI::translateTransition(board, transition) << std::endl;
/*         std::cout <<  chesspieceNotationMap.at(board.pieceAt(transition.srcdest.first)) << 
        boardIndexNotation.at(transition.srcdest.first) << " to " << boardIndexNotation.at(transition.srcdest.second) << transition.instr << std::endl; */
    }

}

std::string BoardConsoleGUI::translateTransition(const Chesslib::Board & board, const Transition & transition) {
    char charP = chesspieceNotationMap.at(board.pieceAt(transition.srcdest.first));
    
    if(charP == 'P') return boardIndexNotation.at(transition.srcdest.first) + boardIndexNotation.at(transition.srcdest.second);        
    else return charP + boardIndexNotation.at(transition.srcdest.first) + boardIndexNotation.at(transition.srcdest.second);
       
}


