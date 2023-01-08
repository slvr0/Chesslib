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


enum class SearchType {
    PERFT = 0, 
    TREE_BRANCH = 1,
    RANDOM_ROLLOUT = 2
};

//some callback structure that can be bound by other
//very simple, one user can attach to the callback
class MoveGenCallback {
public :
    //bind
    inline bool Subscribe(std::function<void(const Board&, const SearchType&, const int &)> ftor) {
        ftor_ = ftor;

        return true;
    }
    //trigger update
    inline void Trigger(const Board& b, SearchType st, int d) {
        /* if(ENABLEDBG)
            m_assert((ftor_), "The callback function has not been defined\n");    */     

        ftor_(b, st, d);
    }

    std::function<void(const Board&, const SearchType&, const int &)> ftor_ = nullptr;    
};

/*
Brief : 

Given a chessboard this class can generate pseudo legal and legal moves from the current state of the board

Dependencies : chessboard_extractor, position_meta_data
*/

//We create a general shell of a movegenerator and deduce specific logic separation for white and black in subclasses , defined in virtual functions
class IMoveGenerator {
public : 
    virtual ~IMoveGenerator() {}

    void GetPseudoLegalMoves(const Board & board, SearchType search_type, int & depth, int & rollout_nr_gen);

    virtual void GetPawnMoves(const Board & board) = 0;
    virtual void GetKnightMoves(const Board & board) = 0;
    virtual void GetBishopMoves(const Board & board) = 0;
    virtual void GetRookMoves(const Board & board) = 0;
    virtual void GetQueenMoves(const Board & board) = 0;
    virtual void GetKingMoves(const Board & board) = 0;
    virtual void SetEnemyOrVoid(const Board& board) = 0;   
    virtual void GetCastlingMoves(const Board& board) = 0;

    inline bool SetCallback(std::function<void(const Board&, const SearchType&, const int &)> ftor) {
        return callback_.Subscribe(ftor);
    }

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
    uint16_t depth_;
    SearchType search_type_;



    std::unique_ptr<IMetaDataRegister> metadata_reg_ = nullptr;
    MoveGenCallback callback_;  

    //for move allocation, remove later ( we dont allocate shit )
    const size_t max_allocation_size_ = 100;
    std::vector<Board> legal_moves_;
    uint16_t search_index_ = 0;
    bool trigger_cb_ = true; //for debug  
};

class WhiteMoveGenerator : public IMoveGenerator {
public : 
    WhiteMoveGenerator();

    inline void SetEnemyOrVoid(const Board& board) override {
        enemy_or_void_ = GetBlackOrVoid(board);
    }

    inline void GetLegalMoves(const Board& board, const SearchType& search_type) {

        movecounter_ = 0;   
        depth_ = 0; //specify input later
        search_type_ = search_type;   
        enemy_or_void_ = ~board.white_;

        RefreshMetaDataInternal(board);

        //if checkmask something then disregard other legal moves right away
        GetKingMoves(board);

        //else proceed these/
        GetPawnMoves(board);
        GetKnightMoves(board);
        GetBishopMoves(board);    
        GetRookMoves(board); 
        GetQueenMoves(board);

        if(nocheck_) GetCastlingMoves(board);     
    }


    void GetPawnMoves(const Board & board);
    void GetKnightMoves(const Board & board);
    void GetBishopMoves(const Board & board);
    void GetRookMoves(const Board & board);
    void GetQueenMoves(const Board & board);
    void GetKingMoves(const Board & board);
    void GetCastlingMoves(const Board& board);

    void RefreshMetaDataInternal(const Board& board);

private:

    BBoard rook_pins_        = { 0x0 };
    BBoard bishop_pins_      = { 0x0 };
    BBoard enp_target_       = { 0x0 };
    BBoard checkmask_        = { 0xffffffffffffffffull };
    BBoard kingban_          = { 0x0 };
    BBoard kingattack_       = { 0x0 }; //current moves for king
    BBoard enemy_kingattack_ = { 0x0 }; //current enemy king attacked squares
    BBoard kingmoves_        = { 0x0 };
    BBoard ekingmoves_       = { 0x0 };
    BBoard check_status_     = { 0x0 };

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

//has white / black mgens , white / black metadata registers and callback for users to act on found moves. also, entry point for searches.
class MoveGeneratorHeader{
public :
    MoveGeneratorHeader() {
        
    }
    
    MoveGeneratorHeader(std::function<void(const Board&, const SearchType&, const int &)> ftor) {
        this->BindCallback(ftor);
    }

    bool BindCallback(std::function<void(const Board&, const SearchType&, const int &)> ftor) {      
        return wmgen_.SetCallback(ftor);
    }

    //interface keeps track of max depth
    inline void PruneSearch_PERFT(const Board& board, int depth) {

        int include_later = 0; // for random rollout move id caps       

        if(board.white_acts_) wmgen_.GetLegalMoves(board, SearchType::PERFT);
        else return;
    }

private:    
    WhiteMoveGenerator              wmgen_;   
   
};

