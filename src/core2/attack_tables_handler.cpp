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
                    occupation_mask *= rook_magic_numbers[idx];
                    occupation_mask >>= 64 - rook_shifts[idx];
                }
                else
                {
                    occupation_mask *= bishop_magic_numbers[idx];
                    occupation_mask >>= 64 - bishop_shifts[idx];

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

uint64_t AttackTablesHandler::get_rook_attacks(const uint64_t & occ, const unsigned long idx)
{

    uint64_t occ_c = occ & rook_mask[idx];

    //uint64_t occ_c = extract_bits(occ, rook_mask[idx]);

    occ_c *= rook_magic_numbers[idx];
    occ_c >>= 64 - rook_shifts[idx];

    return rook_lookup_[idx][occ_c];
}

uint64_t AttackTablesHandler::get_bishop_attacks(const uint64_t & occ, const unsigned long idx)
{
    uint64_t occ_c = occ & bishop_mask[idx];

    occ_c *= bishop_magic_numbers[idx];
    occ_c >>= 64 - bishop_shifts[idx];

    //uint64_t occ_c = extract_bits(occ, rook_mask[idx]);

    return bishop_lookup_[idx][occ];
}