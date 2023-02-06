#pragma once 

#include "defs.h"
#include "chessboard_extractor.h"
#include "static_move_tables.h"

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

FORCEINL BBoard White_Pawn_Forward2(const BBoard & mask) {
	return mask << 16;
}

FORCEINL BBoard Black_Pawn_Forward2(const BBoard & mask) {
	return mask >> 16;
}

FORCEINL BBoard White_Pawn_Backward(const BBoard & mask) {
	return Black_Pawn_Forward(mask);
}

FORCEINL BBoard Black_Pawn_Backward(const BBoard & mask) {
	return White_Pawn_Forward(mask);
}

FORCEINL BBoard White_Pawn_Backward2(const BBoard & mask) {
	return Black_Pawn_Forward2(mask);
}

FORCEINL BBoard Black_Pawn_Backward2(const BBoard & mask) {
	return White_Pawn_Forward2(mask);
}

FORCEINL BBoard White_Pawn_AttackLeft(const BBoard & mask) {
    return mask << 7;	
}

FORCEINL BBoard Black_Pawn_AttackLeft(const BBoard & mask) {
	return mask >> 9;
}

FORCEINL BBoard White_Pawn_AttackRight(const BBoard & mask) {
	return mask << 9;	
}

FORCEINL BBoard Black_Pawn_AttackRight(const BBoard & mask) {
	return mask >> 7;
}

FORCEINL BBoard White_Pawn_InvertLeft(const BBoard & mask) {
	return Black_Pawn_AttackRight(mask);
}

FORCEINL BBoard Black_Pawn_InvertLeft(const BBoard & mask) {
	return White_Pawn_AttackRight(mask);
}

FORCEINL BBoard White_Pawn_InvertRight(const BBoard & mask) {
	return Black_Pawn_AttackLeft(mask);
}

FORCEINL BBoard Black_Pawn_InvertRight(const BBoard & mask) {
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

