#pragma once

#include <memory>
#include <functional>


#include <iostream>
#include <fstream>

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
        wmgen_ = WhiteMoveGenerator(this); 
        bmgen_ = BlackMoveGenerator(this);

        #ifdef _DEBUG
            dbug_file_.open("test_file.txt");

            dbug_file_.clear();
        #endif 
    }

    ~MoveGeneratorHeader() {
        dbug_file_.close();
    }

    //implement me
    inline virtual void OnInsert(const Board& board, const int& depth) {


    }

    inline virtual void OnInsertDebug(const Board& b1, const Board& b2, const std::string & info) {
/*         dbug_file_ << "--------------------------------------------------------ENTRY-------------------------------------------------------------------------------------------\n";
        dbug_file_ <<  info + "\n";
        dbug_file_ << BoardConsoleGUI::BoardAsString(b1);
        dbug_file_ << "\nB Statistics | " << "White Acts : " << b1.white_acts_ << " WhiteCastleKingSide : " << b1.white_oo_ << " WhiteCastleQueenSide : " << b1.white_ooo_ << \
            " BlackCastleKingSide : " << b1.black_oo_ << " BlackCastleQueenSide : " << b1.black_ooo_ << " Enpaissant : " << b1.enp_;       
        dbug_file_ << "\nA Statistics | " << "White Acts : " << b2.white_acts_ << " WhiteCastleKingSide : " << b2.white_oo_ << " WhiteCastleQueenSide : " << b2.white_ooo_ << \
            " BlackCastleKingSide : " << b2.black_oo_ << " BlackCastleQueenSide : " << b2.black_ooo_ << " Enpaissant : " << b2.enp_ << "\n";    
        dbug_file_ << "\n";
        dbug_file_ << BoardConsoleGUI::BoardAsString(b2);
        dbug_file_ << "\n"; */

    }
    
protected:    
    WhiteMoveGenerator              wmgen_;  
    BlackMoveGenerator              bmgen_; 

    std::ofstream dbug_file_;


   
};

