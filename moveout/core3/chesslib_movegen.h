#pragma once

#include <iostream>

#include "chesslib_callback.h"
#include "chesslib_def.h"
#include "chesslib_board.h"
#include "chesslib_boardinfo.h"



namespace Chesslib2 {
class MoveGenerator {

public : 

MoveCallback callback;

template <bool IsWhite>
void SearchPseudoMoves(const Board & board) {

    
    
    //explicit scenario check, we will add hv / d pins and checkmask later

    // HV / D pinmask


    //Checkmask


    //Castling


    //Pawn moves

    //1 move up / down 





    callback.AppendMove();




    std::cout << callback.DisplayNumMoves() << std::endl;

    
}

};
};