#include "v0input_parser.h"


V0InputParser::V0InputParser() {
    
}

template <typename T>
void V0InputParser::SetPartialVector(std::vector<T>& vec, BBoard data, bool white_acts, int offset) {
    std::fill(vec.begin() + offset, vec.begin() + offset + 64 , 0);

    if(white_acts) {        
        data = ReverseBytesInBytes(data);
    }
    else {
        data = ReverseBitsInBytes(data);
    }
    LoopBits(data) {
        vec[offset + LSquare(data)] = 1.0;
    }
}

template <typename T>
void V0InputParser::SetPartialVectorFull(std::vector<T>& vec, int offset) {
    std::fill(vec.begin() + offset, vec.begin() + offset + 64 , 1);
}

template <typename T>
void V0InputParser::SetPartialVectorEmpty(std::vector<T>& vec, int offset) {
    std::fill(vec.begin() + offset, vec.begin() + offset + 64 , 0);
}

cppflow::tensor V0InputParser::BoardToTensor(Board board) const {
    std::vector<std::string> history; //empty
    return this->BoardToTensor(board, history);
}

cppflow::tensor V0InputParser::BoardToTensor(Board board, std::vector<std::string> history) const {
    const bool white_acts = board.white_acts_;
    auto fill_castle_status_fn = [](auto & vec, bool castle_status, int offset) {
        if(castle_status) SetPartialVectorFull(vec, offset);
        else SetPartialVectorEmpty(vec, offset);
    };
    auto fill_enp_fn = [white_acts](auto & vec, int enp, int offset) {
        if(enp == -1) {
            SetPartialVectorEmpty(vec, offset);
        }
        else SetPartialVector(vec, 1ULL << enp, white_acts, offset);          
    };

    int hrat    = (int) 8 * board.half_move_ / 50.0 ;   
    int fmrat   = (int) 8 * board.full_move_ / 100.0 ;
    std::vector<float> vecflat;
    vecflat.resize(32*8*8); 
    int offset = 0;

    if(white_acts) {        
        SetPartialVector(vecflat, board.white_pawn_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.white_knight_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.white_bishop_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.white_rook_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.white_queen_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.white_king_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.black_pawn_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.black_knight_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.black_bishop_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.black_rook_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.black_queen_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.black_king_, white_acts, offset); offset += 64;

        SetPartialVectorEmpty(vecflat, offset); offset += 64;
        SetPartialVectorFull(vecflat, offset); offset += 64;
        SetPartialVectorEmpty(vecflat, offset); offset += 64;

        fill_castle_status_fn(vecflat, board.white_oo_, offset);offset+=64;
        fill_castle_status_fn(vecflat, board.white_ooo_, offset);offset+=64;
        fill_castle_status_fn(vecflat, board.black_oo_, offset);offset+=64;
        fill_castle_status_fn(vecflat, board.black_ooo_, offset);offset+=64;        
    }
    else {
        SetPartialVector(vecflat, board.black_pawn_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.black_knight_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.black_bishop_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.black_rook_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.black_queen_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.black_king_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.white_pawn_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.white_knight_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.white_bishop_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.white_rook_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.white_queen_, white_acts, offset); offset += 64;
        SetPartialVector(vecflat, board.white_king_, white_acts, offset); offset += 64;

        SetPartialVectorEmpty(vecflat, offset); offset += 64;        
        SetPartialVectorEmpty(vecflat, offset); offset += 64;
        SetPartialVectorFull(vecflat, offset); offset += 64;

        fill_castle_status_fn(vecflat, board.black_oo_, offset);offset+=64;
        fill_castle_status_fn(vecflat, board.black_ooo_, offset);offset+=64;
        fill_castle_status_fn(vecflat, board.white_oo_, offset);offset+=64;
        fill_castle_status_fn(vecflat, board.white_ooo_, offset);offset+=64;
    }
        fill_enp_fn(vecflat, board.enp_, offset);offset+=64;

        BBoard fill_halfmove_ratio = static_cast<BBoard>(spatial_occupancy_[std::min(hrat, 4)]);
        BBoard fill_fullmove_ratio = static_cast<BBoard>(spatial_occupancy_[std::min(fmrat, 4)]);

        SetPartialVector(vecflat, fill_halfmove_ratio, noswap_, offset);offset+=64;
        SetPartialVector(vecflat, fill_fullmove_ratio, noswap_, offset);offset+=64;

        //10 history planes
        auto histlen = history.size();
        int planeindex = 0;
        bool running_actor = !white_acts;
        for(auto element : history) {        
            auto index_from = notation_idx(element.substr(0,2).c_str());
            auto index_to = notation_idx(element.substr(2,2).c_str()); 
            SetPartialVector(vecflat, 1ULL << index_from | 1ULL << index_to, running_actor, offset); offset+=64;  
            running_actor = !running_actor;
        }
        if(histlen < 10) {
            int remaining = 10 - histlen;       
            for(int i = remaining; i < 10; ++i) {
                SetPartialVectorEmpty(vecflat, offset);
                offset+=64;
            }
        }

        return cppflow::tensor(vecflat, {32,8,8});
}