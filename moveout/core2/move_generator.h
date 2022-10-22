#pragma once
#include <iostream>

#include "attack_tables_handler.h"
#include "chess_board.h"

#include "x86intrin.h"

namespace Chesslib {

typedef uint64_t U64;

struct Transition {
    std::pair<size_t, size_t> srcdest;
    char instr = 'U'; 
};
typedef std::vector<Transition> Transitions;

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
     /*    for(size_t i = 0 ; i < 100 ; ++i) {
            precashed_entries_[i].reserve(5);
            precashed_legal_entries_[i].reserve(5);            
        } */
    }
    
    const Transitions getMoves(const Chesslib::Board & board);

//invoke private functions to split method
private: 
    void appendPseudoLegalPawnMoves(Transitions& pseduolegal_moves, const int & index, const U64& all_pieces, const U64& enemy_pieces, bool white_acts = true);
    void appendPseudLegalPawnAttacks(Transitions& pseduolegal_moves, const U64& pawns, const U64& enemy_pieces , const bool & white_acts, const BoardState& state);

    LegalMoveMask calculateLegalMoveMask(const size_t& kingindex, const U64& all_pieces, const U64& active_pieces, 
    const U64& pawns,
    const U64& bishops, 
    const U64& knights,
    const U64& rooks,
    const U64& queens,
    bool white_acts,
    const int& enp) const;

    void applyLegalMoveFilter(const LegalMoveMask& pushpinmask, const int & kingindex, const U64 & all_pieces , const U64 & active_pieces, 
        const U64 & enemy_pieces,
        const U64 & full_enemy_attackpattern,
        const BoardState& state,
        const U64& enemy_rooks_queens);

    inline void addEntry(const uint8_t & boardindex, U64 attack_pattern, char spec = 'U') {        
        const size_t popcount = pop_count(attack_pattern);
        for(size_t i = 0 ; i < popcount; ++i) {
            size_t lb = least_bit(attack_pattern);
            precashed_entries_[entries_].srcdest.first = boardindex;
            precashed_entries_[entries_].srcdest.second = lb;
            if(spec != ' ') precashed_entries_[entries_].instr = spec;
            ++entries_;

            attack_pattern -= 1ULL << lb; 
        } 
    }

    inline void addLegalEntry(const uint8_t & boardindex, U64 attack_pattern, char spec = 'U') {
        const size_t popcount = pop_count(attack_pattern);
        for(size_t i = 0 ; i < popcount; ++i) {
            size_t lb = least_bit(attack_pattern);     
            precashed_legal_entries_[legal_entries_].srcdest.first = boardindex;
            precashed_legal_entries_[legal_entries_].srcdest.second = lb;
            if(spec != ' ') precashed_legal_entries_[legal_entries_].instr = spec;
            ++legal_entries_;

            attack_pattern -= 1ULL << lb; 

        } 
        
    }

    inline void addEntrySrcDest(const int & src, const int & dest, char spec = 'U') {
        precashed_entries_[entries_].srcdest.first = src;
        precashed_entries_[entries_].srcdest.second = dest;
        if(spec != ' ') precashed_entries_[entries_].instr = spec;
        ++entries_;
    }

    inline void addLegalEntrySrcDest(const int & src, const int & dest, char spec = 'U') {
        precashed_legal_entries_[legal_entries_].srcdest.first = src;
        precashed_legal_entries_[legal_entries_].srcdest.second = dest;
        if(spec != ' ') precashed_legal_entries_[legal_entries_].instr = spec;
        ++legal_entries_;
    }

/*     inline void adEntry(const uint8_t & boardindex, U64 attack_pattern, const char && spec = 'U') {
        char __s = static_cast<char>(spec);
        addEntry(boardindex, attack_pattern, __s);
    }

    inline void addLegalEntry(const uint8_t & boardindex, U64 attack_pattern, const char && spec = 'U') {
        char __s = static_cast<char>(spec);
        addLegalEntry(boardindex, attack_pattern, __s);
    }

    inline void addEntrySrcDest(const int & src, const int & dest, const char && spec = 'U') {
        char __s = static_cast<char>(spec);
        addEntrySrcDest(src, dest, __s);
    }

    inline void addLegalEntrySrcDest(const int & src, const int & dest, const char && spec = 'U') {
        char __s = static_cast<char>(spec);
        addLegalEntrySrcDest(src, dest, __s);
    } */
    

    //does special check for promotion and adds enpaissant + special pin checks
    void addLegalPawnMove(const size_t & src, const size_t& dest,  const char& instr, const BoardState& state,
    const size_t& kingindex, const U64& active_pieces, const U64& enemy_rooks_queens);


    void addLegalCastlingMove(const LegalMoveMask& pushpinmask, const U64& all_pieces, const BoardState& state, const U64& full_enemy_attackpath);
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
   
    Transitions precashed_entries_ {150};
    Transitions precashed_legal_entries_ {150};

    size_t entries_ = 0;
    size_t legal_entries_ = 0;

    const U64 w00_ = 0x60;
    const U64 w000_ = 0xE;
    const U64 b00_ = 0x6000000000000000;
    const U64 b000_ = 0xE00000000000000;
};
};