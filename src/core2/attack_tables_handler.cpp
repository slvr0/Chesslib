#include "attack_tables_handler.h"

using namespace Chesslib;

void AttackTablesHandler::_init_tables(const std::vector<Direction> &directions, bool rooks)
{
        int nr_filled = 0;
        int offset  = 0;

        auto onboard = [](int row, int col) { return row >= 0 && row < 8 && col >= 0 && col <8;};

        for(int idx = 0 ; idx < 64; idx++)
        {
            std::vector<unsigned long> BB_indices;

            if(rooks)BB_indices = bit_iter(rook_mask[idx]);
            else BB_indices =  bit_iter(bishop_mask[idx]);
    
            for(int i = 0 ; i < 1ULL << BB_indices.size(); ++i)
            {
                uint64_t occupation_mask{0x0};

                for(int k = 0; k < BB_indices.size() ; ++k)
                {
                    if((1 << k) & i)
                    {
                        occupation_mask |= 1ULL << BB_indices.at(k);
                    }
                }

                //set lookup table to the correct memory adress in attack tables
                if(rooks) rook_lookup_[idx] = &rook_attacks_[offset];
                else bishop_lookup_[idx] = &bishop_attacks_[offset];

                uint64_t sq_to_bb{0x0};

                for(const auto & dir : directions)
                {
                    int row = int(idx/8);
                    int col = int(idx % 8);

                    while (true)
                    {
                        row += dir.second;
                        col += dir.first;

                        if (!onboard(row, col)) break;

                        uint64_t ns_64 {1ULL <<  (row*8 + col)};

                        sq_to_bb |= ns_64;

                        if ((ns_64 & occupation_mask) != 0)  break;
                    }
                }

                if(rooks)
                {

                    occupation_mask= u64_pext(occupation_mask, rook_mask[idx]);
                   /*  occupation_mask *= rook_magic_numbers[idx];
                    occupation_mask >>= 64 - rook_shifts[idx]; */
                }
                else
                {
                    occupation_mask = u64_pext(occupation_mask, bishop_mask[idx]);
                    /* occupation_mask *= bishop_magic_numbers[idx];
                    occupation_mask >>= 64 - bishop_shifts[idx]; */

                }

                int n_idx = int(offset + occupation_mask);


                if(rooks && occupation_mask >1ULL <<  rook_shifts[idx]) print("sanity check wrong on rook indexing \n");
                else if(!rooks && occupation_mask > 1ULL <<  bishop_shifts[idx]) print("sanity check wrong on bishop indexing \n");

                if(rooks) rook_attacks_[n_idx] = sq_to_bb;
                else bishop_attacks_[n_idx] = sq_to_bb;

                nr_filled++;
            }

            if(rooks) offset += 1ULL << rook_shifts[idx];
            else offset += 1ULL << bishop_shifts[idx];
        }

        std::string type = rooks ? "Rooks" : "Bishops";
        std::cout << type << " attacks init. " << nr_filled << " sequences inserted \n";      
}

void AttackTablesHandler::_init_kingmoves() {
    
    std::vector<Direction> kd {Direction(1, 1), Direction(1, -1), Direction(-1, 1), Direction(-1, -1), Direction(1, 0), Direction(-1,0), Direction(0,1), Direction(0, -1)};

    for(int i = 0; i < 64 ; ++i)
    {
    uint64_t kmoves {0x0};

    for(const auto & dir : kd)
    {
        int row =int(i / 8) ;
        int col = (i % 8) ;

        row += dir.second;
        col += dir.first;

        if(on_board(row,col)) kmoves |= 1ULL << (row*8 +col);
    }

    king_moves_[i] = kmoves;
    }
}

U64 AttackTablesHandler::getRookAttackPattern(const U64 & occ, const unsigned long idx) const {

    return rook_lookup_[idx][u64_pext(occ, rook_mask[idx])];
}

U64 AttackTablesHandler::getBishopAttackPattern(const U64 & occ, const unsigned long idx) const { 
   
    return bishop_lookup_[idx][u64_pext(occ, bishop_mask[idx])];
}

U64 AttackTablesHandler::getKnightAttackPattern(const unsigned long idx) const {
    return knight_attacks[idx];
}

U64 AttackTablesHandler::getKingMoves(const unsigned long idx) const {
    return king_moves_[idx];    
}

U64 AttackTablesHandler::getPawnAttackPattern(const unsigned long idx, bool white_acts) const {
    return white_acts ? pawn_attacks[idx] : pawn_attacks_rev[idx];
}

