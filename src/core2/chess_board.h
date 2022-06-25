#pragma once

#include <iostream>
#include <vector>
#include <map>

#include "global_utils.cpp"

namespace  Chesslib { 

typedef uint64_t U64;
typedef int64_t S64;
typedef std::pair<int,int> Transition;
typedef std::vector<Transition> Transitions;


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



//additional info on which side owns the piece. Used for locating piece info on certain square.
typedef std::pair<bool, Chesslib::PieceType> SpecificPiece; 

const std::map<SpecificPiece, char> chesspieceNotationMap {
    {SpecificPiece(true, Chesslib::PieceType::INVALID) , ' '}, 
    {SpecificPiece(true, Chesslib::PieceType::PAWN) , 'P'}, 
    {SpecificPiece(true, Chesslib::PieceType::BISHOP) , 'B'}, 
    {SpecificPiece(true, Chesslib::PieceType::KNIGHT) , 'N'}, 
    {SpecificPiece(true, Chesslib::PieceType::ROOK) , 'R'}, 
    {SpecificPiece(true, Chesslib::PieceType::QUEEN) , 'Q'}, 
    {SpecificPiece(true, Chesslib::PieceType::KING) , 'K'}, 

    {SpecificPiece(false, Chesslib::PieceType::INVALID) , ' '}, 
    {SpecificPiece(false, Chesslib::PieceType::PAWN) , 'p'}, 
    {SpecificPiece(false, Chesslib::PieceType::BISHOP) , 'b'}, 
    {SpecificPiece(false, Chesslib::PieceType::KNIGHT) , 'n'}, 
    {SpecificPiece(false, Chesslib::PieceType::ROOK) , 'r'}, 
    {SpecificPiece(false, Chesslib::PieceType::QUEEN) , 'q'}, 
    {SpecificPiece(false, Chesslib::PieceType::KING) , 'k'},
};

const std::vector<std::string> boardIndexNotation {
    "a1", "b1", "c1", "d1", "e1", "f1" ,"g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2" ,"g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3" ,"g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4" ,"g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5" ,"g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6" ,"g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7" ,"g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8" ,"g8", "h8",
};


class Board { 
public : 
    Board() = default;
    Board(std::string fen) {
        setFromFen(fen);
    }
    //this is just for debugging
    Board(const U64 & v) noexcept {
        all_pieces_ |= v;
        white_pieces_ |= v;
        pieces_.pawns |= v;
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
        return white ? pieces &~ black_pieces_ : pieces &~ white_pieces_;
    }

    inline U64 all() const { return all_pieces_;}

    //mostly for debugging right now
    SpecificPiece pieceAt(const int & idx) const;
    
private:
    
    void updateCastleStatus(const int & new_castle_status);

private:
    Pieces pieces_; //specific piece information

    U64 all_pieces_{0x0};
    U64 white_pieces_{0x0}; //collection all white pieces
    U64 black_pieces_{0x0}; //collection all black pieces
    
    BoardState state_;       

};


//helps to visualize the board graphically in console
class BoardConsoleGUI {
public : 

    static void printConsole(const Chesslib::Board & board, const int & print_mode= 0, char spec_type= ' ') {
        auto printer = BoardConsoleGUI::copyBoardContent(board);
        int idx = 56;

        print("___A__B__C__D__E__F__G__H__");

        std::cout << int(idx/8) + 1;

        while( idx >= 0)
        {
        if ((idx + 1) % 8 == 0)
        {
        std::cout << " |" << printer.at(idx) << " |" << std::endl;
        idx -= 15;
        if (int(idx / 8) + 1 > 0)
        {
            std::cout << (int(idx / 8) + 1);
        }
        }
        else
        {
        std::cout << " |" << printer.at(idx);
        idx += 1;
        }
        }   
        print("___A__B__C__D__E__F__G__H__");
        print("\n");
    }

    static std::vector<char> copyBoardContent(const Chesslib::Board & board){
        std::vector<char> printer;
        for (int i = 0 ; i<64 ; ++i)
        {
            Chesslib::SpecificPiece spec_piece = board.pieceAt(i);       
            printer.emplace_back(Chesslib::chesspieceNotationMap.at(spec_piece));
        }
        return printer;
    }

    static void translateTransitions(const Chesslib::Board & board, Transitions & transitions) {

        
        for(const auto & transition : transitions) {
            std::cout <<  chesspieceNotationMap.at(board.pieceAt(transition.first)) << 
            boardIndexNotation.at(transition.first) << " to " << boardIndexNotation.at(transition.second) << std::endl;
        }

    }
};


}; //eof namespace