#pragma once
#include <iostream>
#include <vector>

#include "attack_tables.h"
#include "global_utils.cpp"




namespace Chesslib{
typedef uint64_t U64;

static uint64_t bishop_attacks_ [5248];
static uint64_t rook_attacks_ [102400];
static uint64_t* rook_lookup_[64];
static uint64_t* bishop_lookup_[64];
static uint64_t king_moves_[64];


//the manual way of implementing intels BMI2 paralell bit extract method for fast hashing.
inline U64 u64_pext(U64 val, U64 mask) {
    U64 res = 0;
    for (U64 bb = 1; mask; bb += bb) {
    if ( val & mask & -mask )
        res |= bb;
    mask &= mask - 1;
    }

    
    return res;

    
} 

typedef std::pair<int,int> Direction;

class AttackTablesHandler {
public : 

    AttackTablesHandler() {
        if(!AttackTablesHandler::initialized) {
            std::vector<Direction> bd {Direction(1, 1), Direction(1, -1), Direction(-1, 1), Direction(-1, -1)};
            std::vector<Direction> rd {Direction(1, 0), Direction(-1,0), Direction(0,1), Direction(0, -1)};

            _init_tables(bd, false);
            _init_tables(rd, true);
            _init_kingmoves();
            
            AttackTablesHandler::initialized = true;
        }
    }

    //given a board structure and a rook/bishop on certain idx, returns all the squares the piece can attack.
    U64 getRookAttackPattern(const U64 & occ, const unsigned long idx) const;
    U64 getBishopAttackPattern(const U64 & occ, const unsigned long idx) const;
    U64 getKnightAttackPattern(const unsigned long idx) const;
    U64 getKingMoves(const unsigned long idx) const;
    U64 getPawnAttackPattern(const unsigned long idx, bool white_acts = true) const;

private:
    void _init_tables(const std::vector<Direction> &directions, bool rooks);
    void _init_kingmoves();

private:

    static bool initialized;    
};
};



