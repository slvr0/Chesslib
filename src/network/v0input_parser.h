#ifndef V0INPUT_PARSER
#define V0INPUT_PARSER

#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>

#include <tensorflow/c/c_api.h>
#include <tensorflow/c/tf_tensor.h>
#include <cppflow/cppflow.h>

#include "../blowfish/chessboard_generator.h"
#include "../blowfish/chessboard.h"
/* #include "src/blowfish/chessboard_extractor.h"
#include "src/blowfish/move_generator.h"
#include "src/blowfish/position_meta_data.h"
#include "src/blowfish/perft_divider_autotraverse.h"
#include "src/blowfish/perft_mg_thread.h"

#include "src/chess_interface/movegen_interface.h"


#include "src/mcts_new/mcts_simulation_env.h"
#include "src/mcts_new/mcts_jobqueue.h"

#include "src/network/v0model.h"
#include "src/blowfish/defs.h" */

//mirror observations from black perspective to neural input
inline uint64_t ReverseBitsInBytes(uint64_t v) {
  v = ((v >> 1) & 0x5555555555555555ull) | ((v & 0x5555555555555555ull) << 1);
  v = ((v >> 2) & 0x3333333333333333ull) | ((v & 0x3333333333333333ull) << 2);
  v = ((v >> 4) & 0x0F0F0F0F0F0F0F0Full) | ((v & 0x0F0F0F0F0F0F0F0Full) << 4);
  return v;
}

inline uint64_t ReverseBytesInBytes(uint64_t v) {
  v = (v & 0x00000000FFFFFFFF) << 32 | (v & 0xFFFFFFFF00000000) >> 32;
  v = (v & 0x0000FFFF0000FFFF) << 16 | (v & 0xFFFF0000FFFF0000) >> 16;
  v = (v & 0x00FF00FF00FF00FF) << 8 | (v & 0xFF00FF00FF00FF00) >> 8;
  return v;
}

// Transpose across the diagonal connecting bit 7 to bit 56.
inline uint64_t TransposeBitsInBytes(uint64_t v) {
  v = (v & 0xAA00AA00AA00AA00ULL) >> 9 | (v & 0x0055005500550055ULL) << 9 |
      (v & 0x55AA55AA55AA55AAULL);
  v = (v & 0xCCCC0000CCCC0000ULL) >> 18 | (v & 0x0000333300003333ULL) << 18 |
      (v & 0x3333CCCC3333CCCCULL);
  v = (v & 0xF0F0F0F000000000ULL) >> 36 | (v & 0x000000000F0F0F0FULL) << 36 |
      (v & 0x0F0F0F0FF0F0F0F0ULL);
  return v;
}

/*
Handles conversion from Chessboard / NodeGraph into tensor input for V0 shaped (32,8,8) Neural network.
Holds multiple debug functions also to display input planes content
*/

class TFTensorTools {
public:
    TFTensorTools() {

    }

    //tensor shape shall be (N, _) where 0 <= planarindex < N 
    template <typename T>
    static std::string DebugTensor(cppflow::tensor tensor, int planeindex = 0) {
        auto ret = tensor.get_tensor();
        auto raw_data = TF_TensorData(ret.get());
        size_t size = (TF_TensorByteSize(ret.get()) /
        TF_DataTypeSize(TF_TensorType(ret.get())));

        const auto T_data = static_cast<T*>(raw_data);
        std::vector<T> r(T_data, T_data + size);

        std::string retstr = "";
        for(int i = 0; i < 8; ++i) {
            for(int k = 0; k < 8 ;++k){
                int el = i*8 + k;
                if(r[planeindex*64 + el] == 1) retstr +="X";
                else retstr+= ".";
            }
            retstr += "\n";
        }
        return retstr;
    }

    //tensor shape shall be (N, _) where 0 <= planarindex < N 
    template <typename T>
    static std::string DebugTensor(cppflow::tensor tensor, int && planeindex) {
        auto ret = tensor.get_tensor();
        auto raw_data = TF_TensorData(ret.get());
        size_t size = (TF_TensorByteSize(ret.get()) /
        TF_DataTypeSize(TF_TensorType(ret.get())));

        const auto T_data = static_cast<T*>(raw_data);
        std::vector<T> r(T_data, T_data + size);

        std::string retstr = "";
        for(int i = 0; i < 8; ++i) {
            for(int k = 0; k < 8 ;++k){
                int el = i*8 + k;
                if(r[planeindex*64 + el] == 1) retstr +="X";
                else retstr+= ".";
            }
            retstr += "\n";
        }
        return retstr;
    }

    //V0 format assumed
    template <typename T>
    static std::string DebugV0NeuralInput(cppflow::tensor tensor) {
        std::string ret = "";
        std::vector<std::string> v0_planar_content_ {
        "acting_pawns", "acting_knights", "acting_bishops", "acting_rooks", "acting_queens", "acting_king",
        "observing_pawns", "observing_knights", "observing_bishops", "observing_rooks", "observing_queens", "observing_king",
        "empty_edge","acting_white","acting_black","active_castle_kingside", "active_castle_queenside", "observer_castle_kingside", "observer_castle_queenside",
        "enpaissant","halfmoves","fullmoves",
        "hist0","hist1","hist2","hist3","hist4","hist5","hist6","hist7","hist8","hist9"
        }; 

        for(int i = 0; i < 32; ++i) {
            ret += "PLANE INFO : " + v0_planar_content_[i] + "\n";
            ret += TFTensorTools::DebugTensor<T>(tensor, i);
        }

        return ret;
    }

    static std::string DebugV0NeuralInput(Board board, cppflow::tensor tensor) {

    }

};


class V0InputParser{
public:
    V0InputParser();

    cppflow::tensor BoardToTensor(Board board) const;
    cppflow::tensor BoardToTensor(Board board, std::vector<std::string> history) const;

private:
    template <typename T>
    static void SetPartialVector(std::vector<T>& vec, BBoard data, bool white_acts, int offset);
    template <typename T>
    static void SetPartialVectorFull(std::vector<T>& vec, int offset);
    template <typename T>
    static void SetPartialVectorEmpty(std::vector<T>& vec, int offset);

private:
    unsigned long long spatial_occupancy_ [8]  = {        
        0x0,
        0xFF,
        0xFFFF,
        0xFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFFFF,
        0xFFFFFFFFFFFF,
        0xFFFFFFFFFFFFFF,        
    };
    const bool noswap_ = true;
 
};

#endif