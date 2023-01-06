#pragma once

#include <memory>
#include <functional>

#include "defs.h"
#include "chessboard.h"
#include "chessboard_traverser.h"
#include "chessboard_extractor.h"
#include "static_move_tables.h"
#include "position_meta_data.h"



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
    inline bool Subscribe(std::function<void(const Board&, SearchType, int)> ftor) {
        ftor_ = ftor;

        return true;
    }
    //trigger update
    inline void Trigger(const Board& b, SearchType st, int d) {
        if(ENABLEDBG)
            m_assert((ftor_), "The callback function has not been defined\n");        

        ftor_(b, st, d);
    }

    std::function<void(const Board&, SearchType, int)> ftor_ = nullptr;    
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

    inline bool SetCallback(std::function<void(const Board&, SearchType, int)> ftor) {
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

private:
     
};

//has white / black mgens , white / black metadata registers and callback for users to act on found moves. also, entry point for searches.
class MoveGeneratorHeader{
public :
    MoveGeneratorHeader() {
        
    }
    
    MoveGeneratorHeader(std::function<void(const Board&, SearchType, int)> ftor) {
        this->BindCallback(ftor);
    }

    bool BindCallback(std::function<void(const Board&, SearchType, int)> ftor) {      
        return wmgen_.SetCallback(ftor);
    }

    //interface keeps track of max depth
    inline void PruneSearch_PERFT(const Board& board, int depth) {

        int include_later = 0; // for random rollout move id caps       

        if(board.state_.white_acts_) wmgen_.GetPseudoLegalMoves(board, SearchType::PERFT, depth, include_later);
        else return;
    }

private:    
    WhiteMoveGenerator              wmgen_;
};

