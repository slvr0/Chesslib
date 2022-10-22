#pragma once

#include "defs.h"

#include "chessboard_generator.h"

/*
Brief : 

Provides a class representing a chessboard in a bit format. BoardState is a subcomponent of the Board holding info around castling / enp and who acts. 

*/

struct BoardState {
    const bool white_acts_ = true;
    
    const bool white_oo_ = true;
    const bool white_ooo_ = true;
    const bool black_oo_ = true;
    const bool black_ooo_ = true;

    const int enp_ = -1;

    BoardState() {}

    BoardState(const BoardState& state) :
        white_acts_(state.white_acts_), 
        white_oo_(state.white_oo_), white_ooo_(state.white_ooo_), black_oo_(state.black_oo_), black_ooo_(state.black_ooo_),        
        enp_(state.enp_)
    {

    }

    BoardState(const bool & white_acts, const bool & white_oo, const bool & white_ooo, const bool & black_oo, const bool & black_ooo, const int enp) :
    white_acts_(white_acts), white_oo_(white_oo), white_ooo_(white_ooo), black_oo_(black_oo), black_ooo_(black_ooo), enp_(enp)
    {

    }
};

struct Board {
    const BBoard white_pawn_;
    const BBoard white_knight_;
    const BBoard white_bishop_;
    const BBoard white_rook_;
    const BBoard white_queen_;
    const BBoard white_king_;

    const BBoard black_pawn_;
    const BBoard black_knight_;
    const BBoard black_bishop_;
    const BBoard black_rook_;
    const BBoard black_queen_;
    const BBoard black_king_;    

    const BBoard white_;
    const BBoard black_;    
    const BBoard occ_;

    const BoardState state_;

    Board(
    BBoard white_pawn, BBoard white_knight, BBoard white_bishop, BBoard white_rook, BBoard white_queen, BBoard white_king,
    BBoard black_pawn, BBoard black_knight, BBoard black_bishop, BBoard black_rook, BBoard black_queen, BBoard black_king) :

    white_pawn_(white_pawn), white_knight_(white_knight), white_bishop_(white_bishop), white_rook_(white_rook), white_queen_(white_queen), white_king_(white_king),
    black_pawn_(black_pawn), black_knight_(black_knight), black_bishop_(black_bishop), black_rook_(black_rook), black_queen_(black_queen), black_king_(black_king),

    white_(white_pawn_ | white_knight_ | white_bishop_ | white_rook_ | white_queen_ | white_king_),
    black_(black_pawn_ | black_knight_ | black_bishop_ | black_rook_ | black_queen_ | black_king_),
    occ_(white_ | black_)
    {

    }

    Board(
    BBoard white_pawn, BBoard white_knight, BBoard white_bishop, BBoard white_rook, BBoard white_queen, BBoard white_king,
    BBoard black_pawn, BBoard black_knight, BBoard black_bishop, BBoard black_rook, BBoard black_queen, BBoard black_king, 
    BoardState state) :

    white_pawn_(white_pawn), white_knight_(white_knight), white_bishop_(white_bishop), white_rook_(white_rook), white_queen_(white_queen), white_king_(white_king),
    black_pawn_(black_pawn), black_knight_(black_knight), black_bishop_(black_bishop), black_rook_(black_rook), black_queen_(black_queen), black_king_(black_king),

    white_(white_pawn_ | white_knight_ | white_bishop_ | white_rook_ | white_queen_ | white_king_),
    black_(black_pawn_ | black_knight_ | black_bishop_ | black_rook_ | black_queen_ | black_king_),
    occ_(white_ | black_),
    state_(state)
    {

    }




};