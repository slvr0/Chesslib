#pragma once 
#include <iostream>

#include "chesslib_def.h"
#include "chesslib_board.h"


//extracts data from board

static constexpr uint64_t File1 = 0b1000000010000000100000001000000010000000100000001000000010000000ul;
static constexpr uint64_t File8 = 0b0000000100000001000000010000000100000001000000010000000100000001ul;
static constexpr uint64_t Rank2 = 0b0000000000000000000000000000000000000000000000001111111100000000ul;
static constexpr uint64_t Rank7 = 0b0000000011111111000000000000000000000000000000000000000000000000ul;
static constexpr uint64_t RankMid = 0x0000FFFFFFFF0000;
static constexpr uint64_t Rank_18 = 0xFF000000000000FF;


namespace Chesslib2 {
COMPILETIME BBoard Pawns_NotLeft() {
	return ~File1;
}

COMPILETIME BBoard Pawns_NotRight() {
	return ~File8;
}

FORCEINL BBoard Pawn_Forward(BBoard mask, bool white_acts = true) {
	if (white_acts) return mask << 8;
	else return mask >> 8;
}

FORCEINL BBoard Pawn_Forward2(BBoard mask, bool white_acts = true) {
	if (white_acts) return mask << 16;
	else return mask >> 16;
}

FORCEINL BBoard Pawn_Backward(BBoard mask, bool white_acts = true) {
	return Pawn_Forward(mask, !white_acts);
}

FORCEINL BBoard Pawn_Backward2(BBoard mask, bool white_acts = true) {
	return Pawn_Forward2(mask, !white_acts);
}

FORCEINL BBoard Pawn_AttackLeft(BBoard mask, bool white_acts = true) {
	if (white_acts) return mask << 9;
	else return mask >> 7;
}

FORCEINL BBoard Pawn_AttackRight(BBoard mask, bool white_acts = true) {
	if (white_acts) return mask << 7;
	else return mask >> 9;
}

FORCEINL BBoard Pawn_InvertLeft(BBoard mask, bool white_acts = true) {
	return Pawn_AttackRight(mask, !white_acts);
}

FORCEINL BBoard Pawn_InvertRight(BBoard mask, bool white_acts = true) {
	return Pawn_AttackLeft(mask, !white_acts);
}

FORCEINL BBoard Pawns_FirstRank(bool white_acts = true) {
	if (white_acts) return Rank2;
	else return Rank7;
}

FORCEINL BBoard Pawns_LastRank(bool white_acts = true) {
	if (white_acts) return Rank7;
	else return Rank2;
}

FORCEINL Bit King(const Board& brd) {
	if (brd.state_.white_acts_) return brd.white_king_;
	else return brd.black_king_;
}

FORCEINL Bit EnemyKing(const Board& brd)
{
	if (brd.state_.white_acts_) return brd.black_king_;
	else return brd.white_king_;
}

FORCEINL BBoard Pawns(const Board& brd) {
	if (brd.state_.white_acts_) return brd.white_pawn_;
	else return brd.black_pawn_;
}

FORCEINL BBoard Active(const Board& brd)
{
	if (brd.state_.white_acts_) return brd.white_;
	else return brd.black_;
}
FORCEINL BBoard Enemy(const Board& brd)
{
	if (brd.state_.white_acts_) return brd.black_;
	else return brd.white_;
}

FORCEINL BBoard EnemyRookQueen(const Board& brd)
{
    if (brd.state_.white_acts_) return brd.black_queen_ | brd.black_rook_;
	else return brd.white_queen_ | brd.white_rook_;
}

FORCEINL BBoard RookQueen(const Board& brd)
{
	if (brd.state_.white_acts_) return brd.white_queen_ | brd.white_rook_;
	else return brd.black_queen_ | brd.black_rook_;
}

FORCEINL BBoard EnemyBishopQueen(const Board& brd)
{
	if (brd.state_.white_acts_) return brd.black_queen_ | brd.black_bishop_;
	else return brd.white_queen_ | brd.white_bishop_;
}

FORCEINL BBoard BishopQueen(const Board& brd)
{
	if (brd.state_.white_acts_) return brd.white_queen_ | brd.white_bishop_;
	else return brd.black_queen_ | brd.black_bishop_;
}

FORCEINL BBoard EnemyOrEmpty(const Board& brd)
{	
    if (brd.state_.white_acts_) return ~brd.white_;
	else return ~brd.black_;
}

FORCEINL BBoard Empty(const Board& brd)
{
	return ~brd.occ_;
}

FORCEINL BBoard Knights(const Board& brd)
{
	if (brd.state_.white_acts_) return brd.white_knight_;
	else return brd.black_knight_;
}

FORCEINL BBoard Rooks(const Board& brd)
{
	if (brd.state_.white_acts_) return brd.white_rook_;
	else return brd.black_rook_;
}

FORCEINL BBoard Bishops(const Board& brd)
{
	if (brd.state_.white_acts_) return brd.white_bishop_;
	else return brd.black_bishop_;
}


FORCEINL BBoard Queens(const Board& brd)
{
	if (brd.state_.white_acts_) return brd.white_queen_;
	else return brd.black_queen_;
}
};