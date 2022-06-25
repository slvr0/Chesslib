#include "move_generator.h"
#include "global_utils.cpp"
#include "attack_tables.h"

using namespace Chesslib;



inline void bit_iter_fast(const uint64_t &v )
{
     uint64_t it_t = v;
     int idx_count = pop_count(it_t);

     int shifts = 0;
     for(int i = 0; i < idx_count; ++i)
     {
         unsigned long lb = least_bit(it_t);
         it_t >>= lb + 1;
         shifts += lb + 1;

         //do logic with lb right away;

         if(it_t == 0) break;
     }
}


const Transitions BinaryMoveGenerator::getMoves(const Chesslib::Board & board) {

    //1. make the basic pseudo legal calculations. 

    //2. add special checks (king under attack / pins / push xrays / enp)

    //3. add castle moves


    //get info
    entries_ = 0;
    legal_entries_ = 0;

    Transitions pseudo_legal_transitions;
    const U64 all_pieces = board.all();

    const U64 pawns = board.get(Chesslib::PieceType::PAWN);
    const U64 bishops = board.get(Chesslib::PieceType::BISHOP);
    const U64 knights = board.get(Chesslib::PieceType::KNIGHT);
    const U64 rooks = board.get(Chesslib::PieceType::ROOK);
    const U64 queens = board.get(Chesslib::PieceType::QUEEN);
    const U64 kings = board.get(Chesslib::PieceType::KING);      
    
    Chesslib::BoardState state = board.getState();

    const U64 active_pieces = board.filter(all_pieces, state.white_acts); 
    const U64 enemy_pieces = all_pieces &~ active_pieces;
    const U64 active_king = board.filter(kings, state.white_acts);

    size_t kingposition = 0;  
    
    //2022-06-25 adds 0.4 s per 20 m nodes
    { 
        U64 active_pieces_parse = active_pieces;
        const size_t popcount = pop_count(active_pieces_parse);
        for(size_t i = 0 ; i < popcount; ++i) {
            size_t lb = least_bit(active_pieces_parse);

            U64 lb_64 = 1ULL << lb;

            if(lb_64 & pawns) {
                appendPseudoLegalPawnMoves(pseudo_legal_transitions, lb, all_pieces, enemy_pieces, state.white_acts);
            }
            else if(lb_64 & rooks) {
                U64 attack_pattern = attack_tables_handler_.getRookAttackPattern(all_pieces, lb) & ~active_pieces; 
                addEntry(lb, attack_pattern);                
            }
            else if(lb_64 &  knights) {
                U64 attack_pattern = attack_tables_handler_.getKnightAttackPattern(lb) & ~active_pieces;             
                addEntry(lb, attack_pattern);
            }
            else if(bishops &  lb_64) {
                U64 attack_pattern = attack_tables_handler_.getBishopAttackPattern(all_pieces, lb) & ~active_pieces;  
                addEntry(lb, attack_pattern);         
            }
            else if(queens &  lb_64) {
                U64 attack_pattern = 
                    (attack_tables_handler_.getRookAttackPattern(all_pieces, lb) |
                    attack_tables_handler_.getBishopAttackPattern(all_pieces, lb) ) & ~active_pieces;              
                addEntry(lb, attack_pattern);
            }
            else if(kings &  lb_64) {
                //U64 king_movepattern = attack_tables_handler_.getKingMoves(lb) & ~active_pieces; 
                kingposition = lb;                
                
            }
            active_pieces_parse -= 1ULL << lb;      
        }
    }

    //2022-06-25 adds 0.3 sec per 20 m nodes
    U64 full_enemy_attackpattern = 0x0;
    { //basically only used to know if king can capture attacker. Is there a smarter way to get that info efficiently?
        U64 enemy_pieces_parse = enemy_pieces;
        const size_t popcount = pop_count(enemy_pieces_parse);
        for(size_t i = 0 ; i < popcount; ++i) {
            size_t lb = least_bit(enemy_pieces_parse);

            U64 lb_64 = 1ULL << lb;

            if(lb_64 & pawns) {
                full_enemy_attackpattern |= attack_tables_handler_.getPawnAttackPattern(lb, !state.white_acts); 
            }
            else if(lb_64 & rooks) {
                full_enemy_attackpattern |= attack_tables_handler_.getRookAttackPattern(all_pieces, lb);                               
            }
            else if(lb_64 &  knights) {
                full_enemy_attackpattern |= attack_tables_handler_.getKnightAttackPattern(lb); 
            }
            else if(bishops &  lb_64) {
                full_enemy_attackpattern |= attack_tables_handler_.getBishopAttackPattern(all_pieces, lb); 
            }
            else if(queens &  lb_64) {
                full_enemy_attackpattern |= 
                    (attack_tables_handler_.getRookAttackPattern(all_pieces, lb) |
                    attack_tables_handler_.getBishopAttackPattern(all_pieces, lb) );
            }
            else if(kings &  lb_64) {
                full_enemy_attackpattern |= attack_tables_handler_.getKingMoves(lb);             
            }
            enemy_pieces_parse -= 1ULL << lb;      
        }
    }

    //2022-06-25 adds 0,1 sec per 20 m nodes    
    appendPseudLegalPawnAttacks(pseudo_legal_transitions, pawns, enemy_pieces, state.white_acts);

    //2022-06-25 adds 0,3 sec per 20 m nodes  
    LegalMoveMask pushpinmask = calculateLegalMoveMask(kingposition, all_pieces, active_pieces, pawns, bishops, knights, rooks, queens, state.white_acts);  


    applyLegalMoveFilter(pushpinmask, kingposition, all_pieces, active_pieces, enemy_pieces, full_enemy_attackpattern);
    
    //2022-06-25 adds 0,25 sec per 20 m nodes

    //return Transitions(&precashed_entries_[0] , &precashed_entries_[entries_]);
    return Transitions(&precashed_legal_entries_[0] , &precashed_legal_entries_[legal_entries_]);

    
}

void BinaryMoveGenerator::appendPseudoLegalPawnMoves(Transitions& pseduolegal_moves, const int & index, const U64& all_pieces, const U64& enemy_pieces, bool white_acts) {
    auto append_doublemoves = [&]() {
        return (index < 16 && white_acts) || (index > 47  && !white_acts);
    };

    if(white_acts) {
        //append one move
        if(!((1ULL << index + 8) & all_pieces))  {            
            precashed_entries_[entries_].first = index; 
            precashed_entries_[entries_++].second = index+8; 

            //append doublemove
            if( append_doublemoves() && !((1ULL << index + 16) & all_pieces))  {
                precashed_entries_[entries_].first = index; 
                precashed_entries_[entries_++].second = index+16; 
            }
        }
    }
    else
    {
        //append one move
        if(!((1ULL << index - 8) & all_pieces))  {
            precashed_entries_[entries_].first = index; 
            precashed_entries_[entries_++].second = index-8;  

            //append doublemove
            if(append_doublemoves() && !((1ULL << index - 16) & all_pieces))  {
                precashed_entries_[entries_].first = index; 
                precashed_entries_[entries_++].second = index-16;  
            }
        }
    }
}

void BinaryMoveGenerator::appendPseudLegalPawnAttacks(Transitions& pseduolegal_moves, const U64& pawns, const U64& enemy_pieces , const bool & white_acts)  {
    U64 active_pawns = pawns &~ enemy_pieces;

    const size_t popcount = pop_count(active_pawns);
    for(size_t i = 0 ; i < popcount; ++i) {
        size_t lb = least_bit(active_pawns); 

        U64 pawn_attacks = attack_tables_handler_.getPawnAttackPattern(lb, white_acts) & enemy_pieces; // attack squares requires enemy piece on them
        const size_t attack_pc = pop_count(pawn_attacks);

        addEntry(lb, pawn_attacks);
        active_pawns -= 1ULL << lb; 
    }
}

LegalMoveMask BinaryMoveGenerator::calculateLegalMoveMask(const size_t& kingindex, const U64& all_pieces, const U64& active_pieces, 
    const U64& pawns,
    const U64& bishops, 
    const U64& knights,
    const U64& rooks,
    const U64& queens,
    bool white_acts) const
{

auto origo_rc = [&](){
    return std::pair<int,int>(
        kingindex / 8, 
        kingindex % 8
     );
};

auto on_grid = [&](const int & row , const int & col ) {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
};

U64 enemy_pieces = all_pieces &~ active_pieces;
U64 enemy_pawns = pawns &~ active_pieces;
U64 enemy_rooks = rooks &~ active_pieces;
U64 enemy_bishops = bishops &~ active_pieces;
U64 enemy_queens = queens &~ active_pieces;
U64 enemy_knights = knights &~ active_pieces;

//could we just grab bishop/queens right away
auto positiveInteract = [&](const U64& index_64, const std::pair<int,int> dirr) {
    if((dirr.first != 0 && dirr.second != 0) && ((enemy_bishops | enemy_queens) & index_64)) return true;
    else if(!(dirr.first != 0 && dirr.second != 0) && ((enemy_rooks | enemy_queens) & index_64)) return true;
    else return false;
};

LegalMoveMask pushpinmask;

auto start_rc = origo_rc();

for(const auto & dir : directions_) {
    int r = start_rc.first + dir.second;
    int c = start_rc.second + dir.first;    

    U64 pushmask_dir    = 0x0;
    U64 pinmask_dir     = 0x0;
    U64 capture_dir     = 0x0;

    size_t idx_curr = r * 8 + c;

    bool possible_pinray = false;
    bool possible_pinray_confirmed = false;
    bool enclosed_ray = false;

    while( on_grid(r, c)) {
        //check if we reach an enemy piece. 
        U64 index_64 = 1ULL << idx_curr;

        pushmask_dir    |= index_64;
        pinmask_dir     |= index_64; 

        //enclose the ray information
        if(all_pieces & index_64) {
            if(positiveInteract(index_64, dir)) {
                possible_pinray_confirmed = possible_pinray;
                enclosed_ray = true;
                if(!possible_pinray_confirmed) {
                    capture_dir |= index_64;
                    pushpinmask.attackers += 1; 
                }      
                break;
            }
            //the push mask is resetted, and will be zero, but we need to continue to see if we have pin squares.
            else if(active_pieces & index_64){
                if(possible_pinray) pinmask_dir = index_64; //we already encountered an active piece, so reset the pinray
                possible_pinray = true;
            }
            else { //we intersect with an enemy non slider 
                pushmask_dir    = 0x0;
                pinmask_dir     = 0x0;
                capture_dir     = 0x0;
                break;
            }
        }
        r +=  dir.second;
        c +=  dir.first;
        idx_curr = r * 8 + c;
    }

    if(possible_pinray_confirmed) {        
        pushpinmask.pins.emplace_back(pinmask_dir);
    }
    else if(enclosed_ray) {
        pushpinmask.push |= pushmask_dir;
        pushpinmask.capture |= capture_dir;
    }  
}

//add horsies
auto knight_fullpattern = attack_tables_handler_.getKnightAttackPattern(kingindex);
U64 knight_capture_squares = knight_fullpattern & enemy_knights;
if(knight_capture_squares) {
    pushpinmask.capture |= knight_capture_squares;
    pushpinmask.attackers += 1;    
}

//add pawns 
auto pawn_fullpattern = attack_tables_handler_.getPawnAttackPattern(kingindex, white_acts);
U64 pawn_capture_squares = pawn_fullpattern & enemy_pawns;
if(pawn_capture_squares) {
    pushpinmask.capture |= pawn_capture_squares;
    pushpinmask.attackers += 1;  
}

/* Chesslib::BoardConsoleGUI::printConsole(Board(pushpinmask.push));
Chesslib::BoardConsoleGUI::printConsole(Board(pushpinmask.capture)); */

return pushpinmask;
}

void BinaryMoveGenerator::applyLegalMoveFilter(const LegalMoveMask& pushpinmask, const int & kingindex, const U64 & all_pieces , const U64 & active_pieces, 
const U64 & enemy_pieces,
const U64 & full_enemy_attackpattern)  {  
    //add king moves for respective case
    U64 king_movepattern = attack_tables_handler_.getKingMoves(kingindex);
    king_movepattern &= ~active_pieces;
    king_movepattern &= ~full_enemy_attackpattern;
    king_movepattern &= ~(enemy_pieces & full_enemy_attackpattern);

    if(pushpinmask.attackers > 1) {       
        addLegalEntry(kingindex, king_movepattern);
    }
    else if(pushpinmask.attackers == 1) {
        for(size_t entry = 0; entry < entries_; ++entry) {
            int src = precashed_entries_[entry].first;
            int dest = precashed_entries_[entry].second;
            U64 src64 = 1ULL << src;
            U64 dest64 = 1ULL << dest;

            bool is_pinned = false;
            
            //check if on pinned squares whilst moving to a destination on pinned ray  
            if(!pushpinmask.pins.empty()) {
                for(const auto & pinray : pushpinmask.pins) {
                    if(src64 & pinray) {
                        is_pinned = true;
                        if(dest64 & pinray) {
                            addLegalEntrySrcDest(src, dest);
                            break;
                        }                        
                    }
                }                 
            }

            if(!is_pinned) {
                //is it a block or capture?
                if((dest64 & pushpinmask.push) || (dest64 & pushpinmask.capture)) addLegalEntrySrcDest(src, dest);            
            }
        }
        addLegalEntry(kingindex, king_movepattern);
    }
    else {
        for(size_t entry = 0; entry < entries_; ++entry) {
            bool is_pinned = false;
            int src = precashed_entries_[entry].first;
            int dest = precashed_entries_[entry].second;

            U64 src64 = 1ULL << src;
            U64 dest64 = 1ULL << dest;
            
            //check if on pinned squares whilst moving to a destination on pinned ray  
            if(!pushpinmask.pins.empty()) {                
                for(const auto & pinray : pushpinmask.pins) {
                    if(src64 & pinray) {
                        is_pinned = true;
                        if(dest64 & pinray) {
                            addLegalEntrySrcDest(src, dest);
                            break;
                        }                        
                    }
                }                 
            }            
            if(!is_pinned) addLegalEntrySrcDest(src, dest);
        }  

        /* Chesslib::BoardConsoleGUI::printConsole(Board(pushpinmask.push));    */    
        
        addLegalEntry(kingindex, king_movepattern);
    }

    

    
}







