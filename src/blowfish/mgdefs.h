#pragma once

#include "chessboard.h"
#include "defs.h"

struct MGSearchContextualObject {
    int depth_ = 0;
    size_t movecounter_ = 0;

    BBoard enemy_or_void_     = { 0x0 };

    BBoard checkmask_         = { 0xffffffffffffffffull };
    BBoard rook_pins_         = { 0x0 };
    BBoard bishop_pins_       = { 0x0 };
    BBoard moveable_squares_  = { 0x0 };

    BBoard enp_target_        = { 0x0 };
    BBoard kingban_           = { 0x0 };   
    BBoard kingmoves_         = { 0x0 };
    BBoard ekingmoves_        = { 0x0 };
    BBoard check_status_      = { 0x0 };
    BBoard nocheck_           = { 0x0 }; 

};