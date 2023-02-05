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

static constexpr uint64_t w00_Rookmoves = 0xA0;
static constexpr uint64_t w00_KingMoves = 0x50;
static constexpr uint64_t w000_Rookmoves = 0x9;
static constexpr uint64_t w000_KingMoves = 0x14;

static constexpr uint64_t b00_Rookmoves = 0xA000000000000000;
static constexpr uint64_t b00_KingMoves = 0x5000000000000000;
static constexpr uint64_t b000_Rookmoves = 0x900000000000000;
static constexpr uint64_t b000_KingMoves = 0x1400000000000000;

static constexpr BBoard WSCastleBlockedSquares = 0x60ULL;
static constexpr BBoard WLCastleBlockedSquares = 0xCULL;
static constexpr BBoard BSCastleBlockedSquares = 0x6000000000000000ULL;
static constexpr BBoard BLCastleBlockedSquares = 0xC00000000000000ULL;

static constexpr BBoard WLCastleNoOcc = 0xe;
static constexpr BBoard BLCastleNoOcc = 0xe00000000000000;


//imported
static constexpr uint64_t WNotOccupiedL = 0b01110000ull;
static constexpr uint64_t WNotAttackedL = 0b00111000ull;

static constexpr uint64_t WNotOccupiedR = 0b00000110ull;
static constexpr uint64_t WNotAttackedR = 0b00001110ull;

static constexpr uint64_t BNotOccupiedL = 0b01110000ull << 56ull;
static constexpr uint64_t BNotAttackedL = 0b00111000ull << 56ull;

static constexpr uint64_t BNotOccupiedR = 0b00000110ull << 56ull;
static constexpr uint64_t BNotAttackedR = 0b00001110ull << 56ull;

static constexpr uint64_t WRookL_Change = 0b11111000ull;
static constexpr uint64_t BRookL_Change = 0b11111000ull << 56ull;
static constexpr uint64_t WRookR_Change = 0b00001111ull;
static constexpr uint64_t BRookR_Change = 0b00001111ull << 56ull;

static const uint64_t WRookR = InsertBits(7);
static constexpr uint64_t BRookR = 0b10000000ull << 56ull;
static const uint64_t WRookL = InsertBits(0);
static constexpr uint64_t BRookL = 0b00000001ull << 56ull;

FORCEINL BBoard WhiteEPRank() {
    return 0xFFull << 32;
}

FORCEINL BBoard BlackEPRank() {
    return 0xFFull << 24;
}

FORCEINL BBoard Pawns_NotLeft() {
	return ~File1;
}

FORCEINL BBoard Pawns_NotRight() {
	return ~File8;
}

FORCEINL BBoard White_Pawn_Forward(const BBoard & mask) {
    return mask << 8;
}

FORCEINL BBoard Black_Pawn_Forward(const BBoard & mask) {
    return mask >> 8;
}

FORCEINL BBoard White_Pawn_Forward2(BBoard mask) {
	return mask << 16;
}

FORCEINL BBoard Black_Pawn_Forward2(BBoard mask) {
	return mask >> 16;
}

FORCEINL BBoard White_Pawn_Backward(BBoard mask) {
	return Black_Pawn_Forward(mask);
}

FORCEINL BBoard Black_Pawn_Backward(BBoard mask) {
	return White_Pawn_Forward(mask);
}

FORCEINL BBoard White_Pawn_Backward2(BBoard mask) {
	return Black_Pawn_Forward2(mask);
}

FORCEINL BBoard Black_Pawn_Backward2(BBoard mask) {
	return White_Pawn_Forward2(mask);
}

FORCEINL BBoard White_Pawn_AttackLeft(BBoard mask) {
    return mask << 7;	
}

FORCEINL BBoard Black_Pawn_AttackLeft(BBoard mask) {
	return mask >> 9;
}

FORCEINL BBoard White_Pawn_AttackRight(BBoard mask) {
	return mask << 9;	
}

FORCEINL BBoard Black_Pawn_AttackRight(BBoard mask) {
	return mask >> 7;
}

FORCEINL BBoard White_Pawn_InvertLeft(BBoard mask) {
	return Black_Pawn_AttackRight(mask);
}

FORCEINL BBoard Black_Pawn_InvertLeft(BBoard mask) {
	return White_Pawn_AttackRight(mask);
}

FORCEINL BBoard White_Pawn_InvertRight(BBoard mask) {
	return Black_Pawn_AttackLeft(mask);
}

FORCEINL BBoard Black_Pawn_InvertRight(BBoard mask) {
	return White_Pawn_AttackLeft(mask);
}

FORCEINL BBoard White_Pawns_FirstRank() {
	return Rank2;
}

FORCEINL BBoard Black_Pawns_FirstRank() {
    return Rank7;
}

FORCEINL BBoard White_Pawns_LastRank() {
	return Rank7;	
}

FORCEINL BBoard Black_Pawns_LastRank() {
	return Rank2;
}

FORCEINL void White_Pawns_PruneMove(BBoard & pawn, const BBoard & pinHV) {
    const BBoard pinned = pawn & White_Pawn_Backward(pinHV); //You can forward and are pinned by rook/queen in forward direction
    const BBoard unpinned = pawn & ~pinHV; // You can also move outside the pinned squares.

    pawn = (pinned | unpinned); //You can go forward and you and your targetsquare is allowed
}

FORCEINL void Black_Pawns_PruneMove(BBoard & pawn, const BBoard & pinHV) {
    const BBoard pinned = pawn & Black_Pawn_Backward(pinHV); //You can forward and are pinned by rook/queen in forward direction
    const BBoard unpinned = pawn & ~pinHV; // You can also move outside the pinned squares.

    pawn = (pinned | unpinned); //You can go forward and you and your targetsquare is allowed
}

FORCEINL void White_Pawns_PruneMove2(BBoard & pawn, const BBoard & pinHV) {
    const BBoard pinned = pawn & White_Pawn_Backward2(pinHV); //You can forward and are pinned by rook/queen in forward direction
    const BBoard unpinned = pawn & ~pinHV;

    pawn = (pinned | unpinned); //You can go forward and you and your targetsquare is allowed
}

FORCEINL void Black_Pawns_PruneMove2(BBoard & pawn, const BBoard & pinHV) {
    const BBoard pinned = pawn & Black_Pawn_Backward2(pinHV); //You can forward and are pinned by rook/queen in forward direction
    const BBoard unpinned = pawn & ~pinHV;

    pawn = (pinned | unpinned); //You can go forward and you and your targetsquare is allowed
}

FORCEINL void White_Pawns_PruneLeft(BBoard & pawn, const BBoard & pinD12) {
    const BBoard pinned = pawn & White_Pawn_InvertLeft(pinD12) & Pawns_NotRight(); //You can go left and are pinned
    const BBoard unpinned = pawn & ~pinD12;

    pawn = (pinned | unpinned); //You can go left and you and your targetsquare is allowed 
}

FORCEINL void Black_Pawns_PruneLeft(BBoard & pawn, const BBoard & pinD12) {
    const BBoard pinned = pawn & Black_Pawn_InvertLeft(pinD12) & Pawns_NotRight(); //You can go left and are pinned
    const BBoard unpinned = pawn & ~pinD12;

    pawn = (pinned | unpinned); //You can go left and you and your targetsquare is allowed
}

FORCEINL void White_Pawns_PruneRight(BBoard & pawn, const BBoard & pinD12) {
    const BBoard pinned = pawn & White_Pawn_InvertRight(pinD12) & Pawns_NotLeft(); //You can go left and are pinned
    const BBoard unpinned = pawn & ~pinD12;

    pawn = (pinned | unpinned); //You can go left and you and your targetsquare is allowed
}

FORCEINL void Black_Pawns_PruneRight(BBoard & pawn, const BBoard & pinD12) {
    const BBoard pinned = pawn & Black_Pawn_InvertRight(pinD12) & Pawns_NotLeft(); //You can go left and are pinned
    const BBoard unpinned = pawn & ~pinD12;

    pawn = (pinned | unpinned); //You can go left and you and your targetsquare is allowed
}

FORCEINL void White_Pawn_PruneLeftEP(BBoard& pawn, const BBoard& pinD12)
{
    const BBoard pinned = pawn & White_Pawn_InvertLeft(pinD12) & Pawns_NotRight(); //You can go left and are pinned
    const BBoard unpinned = pawn & ~pinD12;

    pawn = (pinned | unpinned);
}

FORCEINL void White_Pawn_PruneRightEP(BBoard& pawn, const BBoard& pinD12)
{
    const BBoard pinned = pawn & White_Pawn_InvertRight(pinD12) & Pawns_NotLeft(); //You can go right and are pinned
    const BBoard unpinned = pawn & ~pinD12;

    pawn = (pinned | unpinned); 
}

FORCEINL void Black_Pawn_PruneLeftEP(BBoard& pawn, const BBoard& pinD12)
{
    const BBoard pinned = pawn & Black_Pawn_InvertLeft(pinD12) & Pawns_NotRight(); //You can go left and are pinned
    const BBoard unpinned = pawn & ~pinD12;

    pawn = (pinned | unpinned);
}


FORCEINL void Black_Pawn_PruneRightEP(BBoard& pawn, const BBoard& pinD12)
{
    const BBoard pinned = pawn & Black_Pawn_InvertRight(pinD12) & Pawns_NotLeft(); //You can go right and are pinned
    const BBoard unpinned = pawn & ~pinD12;

    pawn = (pinned | unpinned); 
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

/* 

//each move generator (white/black) initiates its own metaregister for specific side of the board
class IMetaDataRegister {
public: 
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

    FORCEINL void CheckBySlider(Square king, Square enemy) {
        if (checkmask == 0xffffffffffffffffull)
        {
            checkmask = Chess_Lookup::PinBetween[king * 64 + enemy]; //Checks are only stopped between king and enemy including taking the enemy
        }
        else checkmask = 0; //If we already have a slider check registered, this means we're now doubleattacked and thus, only available moves are with king
        kingban |= Chess_Lookup::CheckBetween[king * 64 + enemy]; //King cannot go to square opposite to slider 
    }

    virtual void            InitMetaDataSearch(const Board& board) = 0;  
    virtual void            RefreshMetaData(const Board & board) = 0;  

    FORCEINL virtual void   RegisterPinHorisontalVertical(const Square & king, const Square & enemy, const Board& board) = 0;
    FORCEINL virtual void   RegisterPinDiagonal(Square king, Square enemy, const Board& board) = 0;    
    
};



//remember RegisterEPPin function in future.
class WhiteMetaDataRegister : public IMetaDataRegister{
public : 

    WhiteMetaDataRegister() {
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
    }
    
private :

    
    virtual void InitMetaDataSearch(const Board& board) override;  
    virtual void RefreshMetaData(const Board & board) override;

    FORCEINL virtual void RegisterPinHorisontalVertical(const Square & king, const Square & enemy, const Board& board) override {
        const BBoard pin_mask = Chess_Lookup::PinBetween[king * 64 + enemy];

        if (pin_mask & board.white_) {
            rook_pins |= pin_mask;
        }
    }

    FORCEINL virtual void RegisterPinDiagonal(Square king, Square enemy, const Board& board) override {
        const BBoard pin_mask = Chess_Lookup::PinBetween[king * 64 + enemy];

        // https://lichess.org/editor?fen=6q1%2F8%2F8%2F3pP3%2F8%2F1K6%2F8%2F8+w+-+-+0+1
        if (board.enp_ != -1) {
            if (pin_mask & (1ULL << board.enp_)) enp_target = 0;
        }

        if (pin_mask & board.white_) {
            bishop_pins |= pin_mask;
        }
    }

    
};

 */