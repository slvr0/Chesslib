#pragma once

#include <iostream>

namespace  Chesslib { 

typedef uint64_t U64;
typedef int64_t S64;

struct Pieces{
    U64 pawns{};
    U64 knights{};
    U64 bishops_queens{};
    U64 rooks_queens{};
    U64 kings{};
};

struct BoardState {
    bool        white_acts = true;
    uint32_t    move_nr = 0;
    std::string fen{};
    uint8_t     castle{0x00};
    std::string enp{"-"};
    short       rule50{0};
    short       total_moves{0};
};

enum PieceType {
    PAWN = 1,
    KNIGHT = 2, 
    BISHOP = 3, 
    ROOK = 4, 
    QUEEN = 5, 
    KING = 6,
    INVALID = 0
};

class Board { 
public : 
    Board() = default;
    Board(std::string fen) {
        setFromFen(fen);
    }

    Board(const Board &) = delete; // gotta think about this
    Board& operator =(Board & rhv) = delete; 

    inline U64 get(const PieceType type) const {
        switch(type) {
            case Chesslib::PAWN :   return pieces_.pawns;
            case Chesslib::BISHOP : return pieces_.bishops_queens & ~pieces_.rooks_queens;
            case Chesslib::KNIGHT : return pieces_.knights;
            case Chesslib::ROOK :   return pieces_.rooks_queens & ~pieces_.bishops_queens;
            case Chesslib::QUEEN :  return pieces_.rooks_queens & pieces_.bishops_queens;
            case Chesslib::KING :   return pieces_.kings;
            default :               return 0x0;           
        }
    }

    bool setFromFen(std::string fen);
    void setHashValue();

    //delete old piece sq, adds in new square, updates ev. castle
    void applyMove(const std::string uci_move);
    void applyMove(const int nn_move_index);

    void insertPiece(const int & idx, Chesslib::PieceType type, bool ours = true);
    void removePiece(const int & idx, Chesslib::PieceType type, bool ours = true);

    PieceType translatePieceType(char c);

    inline BoardState getState() const { return state_ ;}

    //applies a side filter on any extracted piecetype values
    inline U64 filter(const U64& pieces, bool white = true) const {
        return white ? pieces & white_pieces_ : pieces & black_pieces_;
    }
    
private:
    
    void updateCastleStatus(const int & new_castle_status);

private:
    Pieces pieces_; //specific piece information

    U64 white_pieces_{0x0}; //collection all white pieces
    U64 black_pieces_{0x0}; //collection all black pieces
    
    BoardState state_;       

};
}; //eof namespace