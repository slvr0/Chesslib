#pragma once

#include <memory>

#include "defs.h"
#include "chessboard.h"


class IMetaDataRegister;

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
    virtual void GetCastlingMoves(const Board& board) = 0;


protected : 
    BBoard enemy_or_void_ = 0x0;
    int movecounter_ = 0;

    BBoard checkmask_;
    BBoard rook_pins_;
    BBoard bishop_pins_;
    BBoard moveable_squares_;
    BBoard kingban_;
    BBoard enp_target_;
    bool   nocheck_; 

    std::unique_ptr<IMetaDataRegister> metadata_reg_ = nullptr;

};

class WhiteMoveGenerator : public IMoveGenerator {
public : 
    WhiteMoveGenerator();

    virtual void SetEnemyOrVoid(const Board& board) override;
    virtual void GetPawnMoves(const Board & board) override;
    virtual void GetKnightMoves(const Board & board) override;
    virtual void GetBishopMoves(const Board & board) override;
    virtual void GetRookMoves(const Board & board) override;
    virtual void GetQueenMoves(const Board & board) override;
    virtual void GetKingMoves(const Board & board) override;
    virtual void GetCastlingMoves(const Board& board) override;
};