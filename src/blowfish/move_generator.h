#pragma once

#include <memory>
#include <functional>

#include "defs.h"
#include "chessboard.h"
#include "chessboard_traverser.h"
#include "chessboard_extractor.h"
#include "static_move_tables.h"
#include "position_meta_data.h"

#include "dubious.h"

//remove later, move gen doesnt care, literally
enum class SearchType {
    PERFT = 0, 
    TREE_BRANCH = 1,
    RANDOM_ROLLOUT = 2
};

class MoveGeneratorHeader;

class WhiteMoveGenerator {
public : 
    WhiteMoveGenerator(MoveGeneratorHeader* parent = nullptr);

    void GetPawnMoves(const Board & board);
    void GetKnightMoves(const Board & board);
    void GetBishopMoves(const Board & board);
    void GetRookMoves(const Board & board);
    void GetQueenMoves(const Board & board);
    void GetKingMoves(const Board & board);
    void GetCastlingMoves(const Board& board);

    void RefreshMetaDataInternal(const Board& board);    

    void ParseLegalMoves(const Board& board, const int& depth);  

private:
    int depth_ = 0;
    size_t movecounter_ = 0;
    SearchType search_type_;
    BBoard enemy_or_void_;

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

    MoveGeneratorHeader* parent_ = nullptr;

    FORCEINL void CheckBySlider(const Square& king,const Square& enemy) {
        if (checkmask_ == 0xffffffffffffffffull)
        {
            checkmask_ = Chess_Lookup::PinBetween[king * 64 + enemy]; //Checks are only stopped between king and enemy including taking the enemy
        }
        else checkmask_ = 0; //If we already have a slider check registered, this means we're now doubleattacked and thus, only available moves are with king
        kingban_ |= Chess_Lookup::CheckBetween[king * 64 + enemy]; //King cannot go to square opposite to slider 
    }

    FORCEINL void RegisterPinHorisontalVertical(const Square & king, const Square & enemy, const Board& board) {
        const BBoard pin_mask = Chess_Lookup::PinBetween[king * 64 + enemy];

        if (pin_mask & board.white_) {
        rook_pins_ |= pin_mask;
        }
    }

    FORCEINL void RegisterPinDiagonal(const Square& king, const Square& enemy, const Board& board) {
        const BBoard pin_mask = Chess_Lookup::PinBetween[king * 64 + enemy];

        // https://lichess.org/editor?fen=6q1%2F8%2F8%2F3pP3%2F8%2F1K6%2F8%2F8+w+-+-+0+1
        if (board.enp_ != -1) {
            if (pin_mask & (1ULL << board.enp_)) enp_target_ = 0;
        }

        if (pin_mask & board.white_) {
        bishop_pins_ |= pin_mask;
        }
    }
     
};


//this class will be user implemented to tailor the need for search. Question is what is needed in order to work with OnInsert. 

//PUREPERFT , cares about depth
//SIMPLEEXPAND, cares about nothing just collects found moves
//TREE_EXP cares about nothing , internally keeps track of where to insert
//TREE_ROLL cares about current move id.

// Conclude to cover these needs we simply require to return depth and all needs can be overrided

class MoveGeneratorHeader{
public :
    MoveGeneratorHeader() {
        wmgen_ = WhiteMoveGenerator(this); //reinitialize like a punk
    }

    //implement me
    inline virtual void OnInsert(const Board& board, const int& depth) {
        
    }

protected:    
    WhiteMoveGenerator              wmgen_;   
   
};

