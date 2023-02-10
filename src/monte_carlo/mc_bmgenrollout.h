#pragma once

//dirty , but we need to redo a movegenerator for rollout purposes without callbacks

#include "../blowfish/defs.h"
#include "../blowfish/chessboard.h"
#include "../blowfish/chessboard_traverser.h"
#include "../blowfish/chessboard_extractor.h"
#include "../blowfish/static_move_tables.h"
#include "../blowfish/position_meta_data.h"
#include "../blowfish/mgdefs.h"
#include "mc_defs.h"

namespace MCTS {

class BlackRolloutMoveGenerator {
public : 
    BlackRolloutMoveGenerator() {
        
    }

    MGSearchContextualObject RefreshMetaDataInternal(const Board& board);   

    RMGResult ParseLegalMoves(const Board& board, const int& select_id, MGSearchContextualObject* rerun = nullptr);     

private:

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

    FORCEINL bool select_increment(const int& select_id, const int & N) {
        return select_id == N;
    }

    
     
};
}

