#pragma once 

#include "defs.h"
#include "chessboard.h"
#include "chessboard_traverser.h"
#include "chessboard_extractor.h"
#include "static_move_tables.h"
#include "position_meta_data.h"

#include "mgdefs.h"

class MoveGeneratorHeader;

class BlackMoveGenerator {
public : 
    BlackMoveGenerator(MoveGeneratorHeader* parent = nullptr);

    void GetPawnMoves(const Board & board,      MGSearchContextualObject & context);
    void GetKnightMoves(const Board & board,    MGSearchContextualObject & context);
    void GetBishopMoves(const Board & board,    MGSearchContextualObject & context);
    void GetRookMoves(const Board & board,      MGSearchContextualObject & context);
    void GetQueenMoves(const Board & board,     MGSearchContextualObject & context);
    void GetKingMoves(const Board & board,      MGSearchContextualObject & context);
    void GetCastlingMoves(const Board& board,   MGSearchContextualObject & context);

    MGSearchContextualObject RefreshMetaDataInternal(const Board& board);   

    bool ParseLegalMoves(const Board& board, const int& depth, const bool& debug = false);  

    double metadata_searchtime = 0; //remove later 

private:

    MoveGeneratorHeader* parent_ = nullptr;

    FORCEINL void CheckBySlider(const Square& king,const Square& enemy, MGSearchContextualObject & context) {
        if (context.checkmask_ == 0xffffffffffffffffull)
        {
            context.checkmask_ = Chess_Lookup::PinBetween[king * 64 + enemy]; //Checks are only stopped between king and enemy including taking the enemy
        }
        else context.checkmask_ = 0; //If we already have a slider check registered, this means we're now doubleattacked and thus, only available moves are with king
        context.kingban_ |= Chess_Lookup::CheckBetween[king * 64 + enemy]; //King cannot go to square opposite to slider 
    }

    FORCEINL void RegisterPinHorisontalVertical(const Square & king, const Square & enemy, const Board& board, MGSearchContextualObject & context) {
        const BBoard pin_mask = Chess_Lookup::PinBetween[king * 64 + enemy];

        if (pin_mask & board.black_) {
            context.rook_pins_ |= pin_mask;
        }
    }

    FORCEINL void RegisterPinDiagonal(const Square& king, const Square& enemy, const Board& board, MGSearchContextualObject & context) {
        const BBoard pin_mask = Chess_Lookup::PinBetween[king * 64 + enemy];

        // https://lichess.org/editor?fen=6q1%2F8%2F8%2F3pP3%2F8%2F1K6%2F8%2F8+w+-+-+0+1
        if (board.enp_ != -1) {
            if (pin_mask & (1ULL << board.enp_)) context.enp_target_ = 0;
        }

        if (pin_mask & board.black_) {
            context.bishop_pins_ |= pin_mask;
        }
    }

     
};