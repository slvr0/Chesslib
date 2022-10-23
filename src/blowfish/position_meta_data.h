#pragma once 

#include "defs.h"
#include "chessboard_extractor.h"
#include "static_move_tables.h"

/*
Brief : 

Intended to generate meta data such as checkmasks , pins and castling oppertunities necessary for the move generator to function

*/

static constexpr uint64_t File1 = 0b1000000010000000100000001000000010000000100000001000000010000000ul;
static constexpr uint64_t File8 = 0b0000000100000001000000010000000100000001000000010000000100000001ul;
static constexpr uint64_t Rank2 = 0b0000000000000000000000000000000000000000000000001111111100000000ul;
static constexpr uint64_t Rank7 = 0b0000000011111111000000000000000000000000000000000000000000000000ul;
static constexpr uint64_t RankMid = 0x0000FFFFFFFF0000;
static constexpr uint64_t Rank_18 = 0xFF000000000000FF;

FORCEINL BBoard Pawns_NotLeft() {
	return ~File1;
}

FORCEINL BBoard Pawns_NotRight() {
	return ~File8;
}

FORCEINL BBoard Pawn_Forward(BBoard mask, const bool & white) {
	if (white) return mask << 8;
	else return mask >> 8;
}

FORCEINL BBoard Pawn_Forward2(BBoard mask , const bool & white) {
	if (white) return mask << 16;
	else return mask >> 16;
}

FORCEINL BBoard Pawn_Backward(BBoard mask, const bool & white) {
	return Pawn_Forward(mask, !white);
}

FORCEINL BBoard Pawn_Backward2(BBoard mask, const bool & white) {
	return Pawn_Forward2(mask, !white);
}

FORCEINL BBoard Pawn_AttackLeft(BBoard mask, const bool & white) {
	if (white) return mask << 9;
	else return mask >> 7;
}

FORCEINL BBoard Pawn_AttackRight(BBoard mask, const bool & white) {
	if (white) return mask << 7;
	else return mask >> 9;
}

template<bool IsWhite>
FORCEINL BBoard Pawn_InvertLeft(BBoard mask, const bool & white) {
	return Pawn_AttackRight(mask, !white);
}

FORCEINL BBoard Pawn_InvertRight(BBoard mask, const bool & white) {
	return Pawn_AttackLeft(mask, !white);
}

constexpr BBoard Pawns_FirstRank(const bool & white) {
	if (white) return Rank2;
	else return Rank7;
}

constexpr BBoard Pawns_LastRank(const bool & white) {
	if (white) return Rank7;
	else return Rank2;
}

namespace Movestack 
{
    //Can be removed - incremental bitboard to save some slider lookups is more expensive then lookup itself. So this release does not have a changemap
    static inline Square Atk_King[32];  //Current moves for current King
    static inline Square Atk_EKing[32]; //Current enemy king attacked squares

    static inline BBoard Check_Status[32];   //When a pawn or a knight does check we can assume at least one check. And only one (initially) since a pawn or knight cannot do discovery
}

// When an initial position search is queued, the additional checkmask / kingban / king attack need to be understood. 
// When the movegenerator searches through a position, it shall append information regarding these variables
// such that when MetaData is refreshed for the following position, these properties are already known

// pins need to be searched regardless for each new position.

class MetaDataRegister {
public : 
    BBoard rook_pins        = { };
    BBoard bishop_pins      = { };
    BBoard enp_target       = { };
    BBoard checkmask        = { 0xffffffffffffffffull };
    BBoard kingban          = { };
    BBoard kingattack       = { }; //current moves for king
    BBoard enemy_kingattack = { }; //current enemy king attacked squares

    BBoard kingmoves = { 0x0 };
    BBoard ekingmoves = { 0x0 };
    BBoard check_status = { 0x0 };



    void RefreshMetaData(const Board & board);
    void AppendOnTraversal();
    void InitMetaDataSearch(const Board& board);

private :

    FORCEINL void RegisterPinHorisontalVertical(const Square & king, const Square & enemy, const Board& board) {
        const BBoard pin_mask = Chess_Lookup::PinBetween[king * 64 + enemy];

        if (pin_mask & GetActive(board)) {
            rook_pins |= pin_mask;
        }
    }

    FORCEINL void RegisterPinDiagonal(Square king, Square enemy, const Board& board)
    {
        const BBoard pin_mask = Chess_Lookup::PinBetween[king * 64 + enemy];

        // https://lichess.org/editor?fen=6q1%2F8%2F8%2F3pP3%2F8%2F1K6%2F8%2F8+w+-+-+0+1
        if (board.state_.enp_ != -1) {
            if (pin_mask & (1ULL << board.state_.enp_)) enp_target = 0;
        }

        if (pin_mask & GetActive(board)) {
            bishop_pins |= pin_mask;
        }
    }

    FORCEINL void CheckBySlider(Square king, Square enemy) {
        if (checkmask == 0xffffffffffffffffull)
        {
            checkmask = Chess_Lookup::PinBetween[king * 64 + enemy]; //Checks are only stopped between king and enemy including taking the enemy
        }
        else checkmask = 0; //If we already have a slider check registered, this means we're now doubleattacked and thus, only available moves are with king
        kingban |= Chess_Lookup::CheckBetween[king * 64 + enemy]; //King cannot go to square opposite to slider 
        
    
    }

    





};

