#pragma once

#include <memory>
#include <functional>

#include "defs.h"
#include "chessboard.h"
#include "chessboard_traverser.h"
#include "chessboard_extractor.h"
#include "static_move_tables.h"
#include "position_meta_data.h"

#include "wmovegen.h"
#include "bmovegen.h"

//this class will be user implemented to tailor the need for search. Question is what is needed in order to work with OnInsert. 

//PUREPERFT , cares about depth
//SIMPLEEXPAND, cares about nothing just collects found moves
//TREE_EXP cares about nothing , internally keeps track of where to insert
//TREE_ROLL cares about current move id.

// Conclude to cover these needs we simply require to return depth and all needs can be overrided

class MoveGeneratorHeader{
public :
    MoveGeneratorHeader() {
        wmgen_ = WhiteMoveGenerator(this); //reinitialize like a punk
        bmgen_ = BlackMoveGenerator(this); //reinitialize like a punk
    }

    //implement me
    inline virtual void OnInsert(const Board& board, const int& depth) {
        
    }

protected:    
    WhiteMoveGenerator              wmgen_;  
    BlackMoveGenerator              bmgen_; 
   
};

