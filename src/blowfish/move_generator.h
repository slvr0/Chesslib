#pragma once

#include "defs.h"
#include "chessboard.h"

/*
Brief : 

Given a chessboard this class can generate pseudo legal and legal moves from the current state of the board

Dependencies : chessboard_extractor, position_meta_data
*/

//We create a general shell of a movegenerator and deduce specific logic separation for white and black in subclasses , defined in virtual functions
class IMoveGenerator {
public : 
    virtual ~IMoveGenerator() {}

    void GetPseudoLegalMoves(const Board & board);


    virtual void GetPawnMoves(const Board & board) = 0;
    virtual void GetKnightMoves(const Board & board) = 0;
    virtual void GetBishopMoves(const Board & board) = 0;
    virtual void GetRookMoves(const Board & board) = 0;
    virtual void GetQueenMoves(const Board & board) = 0;
    virtual void GetKingMoves(const Board & board) = 0;
    virtual void SetEnemyOrVoid(const Board& board) = 0;   


protected : 
    BBoard enemy_or_void_ = 0x0;
    int movecounter_ = 0;
    
};

class WhiteMoveGenerator : public IMoveGenerator {
public : 
    WhiteMoveGenerator() {

    }

    virtual void SetEnemyOrVoid(const Board& board) override;
    virtual void GetPawnMoves(const Board & board) override;
    virtual void GetKnightMoves(const Board & board) override;
    virtual void GetBishopMoves(const Board & board) override;
    virtual void GetRookMoves(const Board & board) override;
    virtual void GetQueenMoves(const Board & board) override;
    virtual void GetKingMoves(const Board & board) override;

};