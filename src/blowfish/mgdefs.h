#pragma once

#include "chessboard.h"
#include "defs.h"

struct MGSearchContextualObject {
    int     depth_              = 0;    

    BBoard  enemy_or_void_      = { 0x0 };

    BBoard  checkmask_          = { 0xffffffffffffffffull };
    BBoard  rook_pins_          = { 0x0 };
    BBoard  bishop_pins_        = { 0x0 };
    BBoard  moveable_squares_   = { 0x0 };   

    BBoard  enp_target_         = { 0x0 };
    BBoard  kingban_            = { 0x0 };   
    BBoard  kingmoves_          = { 0x0 };
    BBoard  ekingmoves_         = { 0x0 };
    BBoard  check_status_       = { 0x0 };
    bool    nocheck_            = false; 

    bool    debug_              = false;

};

struct PositionStatus {

    PositionStatus(const bool& terminal, const bool& nocheck) : 
        terminal_(terminal), nocheck_(nocheck) {

    }

    PositionStatus(const bool& terminal, const bool& nocheck, const int& nmoves) : 
        terminal_(terminal), nocheck_(nocheck), nmoves_(nmoves) {

    }

    bool    terminal_;
    bool    nocheck_;   
    int     nmoves_;     
};