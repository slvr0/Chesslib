#pragma once
#include <iostream>

#include "attack_tables_handler.h"
#include "chess_board.h"

namespace Chesslib {

typedef std::pair<int,int> Transition;
typedef std::vector<Transition> Transitions;
typedef uint64_t U64;

struct LegalMoveMask {
    U64     capture = 0x0;
    U64     push = 0x0;
    std::vector<U64> pins; // first = pin squares on ray, second = source  
    size_t  attackers = 0;
};

//collects the information from a Chessboard and returns the possible binary transitions available
class BinaryMoveGenerator {

public :
    BinaryMoveGenerator() {
        for(size_t i = 0 ; i < 100 ; ++i) {
            precashed_entries_[i].first = 0;
            precashed_entries_[i].second = 0;
        }
    }
    
    const Transitions getMoves(const Chesslib::Board & board);

//invoke private functions to split method
private: 
    void appendPseudoLegalPawnMoves(Transitions& pseduolegal_moves, const int & index, const U64& all_pieces, const U64& enemy_pieces, bool white_acts = true);
    void appendPseudLegalPawnAttacks(Transitions& pseduolegal_moves, const U64& pawns, const U64& enemy_pieces , const bool & white_acts);

    LegalMoveMask calculateLegalMoveMask(const size_t& kingindex, const U64& all_pieces, const U64& active_pieces, 
    const U64& pawns,
    const U64& bishops, 
    const U64& knights,
    const U64& rooks,
    const U64& queens,
    bool white_acts = true) const;

    void applyLegalMoveFilter(const LegalMoveMask& pushpinmask, const int & kingindex, const U64 & all_pieces , const U64 & active_pieces, 
        const U64 & enemy_pieces,
        const U64 & full_enemy_attackpattern);

    inline void addEntry(const uint8_t & boardindex, U64 attack_pattern) {
        const size_t popcount = pop_count(attack_pattern);
        for(size_t i = 0 ; i < popcount; ++i) {
        size_t lb = least_bit(attack_pattern);


        precashed_entries_[entries_].first = boardindex;
        precashed_entries_[entries_++].second = lb; 
        attack_pattern -= 1ULL << lb;      
        } 
    }

    inline void addLegalEntry(const uint8_t & boardindex, U64 attack_pattern) {
        const size_t popcount = pop_count(attack_pattern);
        for(size_t i = 0 ; i < popcount; ++i) {
        size_t lb = least_bit(attack_pattern);

        precashed_legal_entries_[legal_entries_].first = boardindex;
        precashed_legal_entries_[legal_entries_++].second = lb; 
        attack_pattern -= 1ULL << lb;      
        } 
    }

    inline void addLegalEntrySrcDest(const int & src, const int & dest) {
        precashed_legal_entries_[legal_entries_].first = src;
        precashed_legal_entries_[legal_entries_++].second = dest;             
        
    }

private:
    Chesslib::AttackTablesHandler attack_tables_handler_;

    std::vector<std::pair<int,int>> directions_ { 
    std::pair<int,int> (1,0),
    std::pair<int,int> (-1,0),
    std::pair<int,int> (0,1),
    std::pair<int,int> (0,-1),
    std::pair<int,int> (1,1),
    std::pair<int,int> (1,-1),
    std::pair<int,int> (-1,1),
    std::pair<int,int> (-1,-1)    
    };
   
    Transitions precashed_entries_ {100};
    Transitions precashed_legal_entries_ {100};

    size_t entries_ = 0;
    size_t legal_entries_ = 0;
};
};