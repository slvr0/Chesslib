#include <iostream>

#include <vector>
#include <random>
#include <cstdlib>

#include <tensorflow/c/c_api.h>
#include <tensorflow/c/tf_tensor.h>

#include <cppflow/cppflow.h>

#include "src/blowfish/chessboard_generator.h"
#include "src/blowfish/chessboard_extractor.h"
#include "src/blowfish/move_generator.h"
#include "src/blowfish/position_meta_data.h"
#include "src/blowfish/perft_divider_autotraverse.h"
#include "src/blowfish/perft_mg_thread.h"

#include "src/chess_interface/movegen_interface.h"
#include "src/blowfish/chessboard.h"

#include "src/mcts_new/mcts_simulation_env.h"
#include "src/mcts_new/mcts_jobqueue.h"

#include "src/network/v0model.h"
#include "src/blowfish/defs.h"

#include <initializer_list>

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

std::vector<float> CreateVecFromBBoard(BBoard origin, bool white_acts = true) {
    std::vector<float> ret(64);
    std::fill(ret.begin(), ret.end(), 0);

    if(white_acts) {        
        origin = ReverseBytesInBytes(origin); //this works for white!
    }
    else {
        origin = ReverseBitsInBytes(origin);
    }
    LoopBits(origin) {ret[LSquare(origin)] = 1.0;}
    return ret;
}
template <typename T>
void SetPartialVector(std::vector<T>& vec, BBoard data, bool white_acts, int offset) {
    std::fill(vec.begin() + offset, vec.begin() + offset + 64 , 0);

    if(white_acts) {        
        data = ReverseBytesInBytes(data); //this works for white!
    }
    else {
        data = ReverseBitsInBytes(data);
    }
    LoopBits(data) {
        vec[offset + LSquare(data)] = 1.0;
    }
}

template <typename T>
void SetPartialVectorFull(std::vector<T>& vec, int offset) {
    std::fill(vec.begin() + offset, vec.begin() + offset + 64 , 1);
}

template <typename T>
void SetPartialVectorEmpty(std::vector<T>& vec, int offset) {
    std::fill(vec.begin() + offset, vec.begin() + offset + 64 , 0);
}

//call on multidimensional input to get plane index
template <typename T>
std::string DebugTensor(cppflow::tensor tensor, int planeindex = 0) {
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

void PrepareInput(Board board, cppflow::model& model)
{ 
    std::vector<std::string> planar_content {
        "acting_pawns", "acting_knights", "acting_bishops", "acting_rooks", "acting_queens", "acting_king",
        "observing_pawns", "observing_knights", "observing_bishops", "observing_rooks", "observing_queens", "observing_king",
        "empty_edge","acting_white","acting_black","enpaissant","halfmoves","fullmoves",
        "hist0","hist1","hist2","hist3","hist4","hist5","hist6","hist7","hist8","hist9"
    };

    

    //print(planar_content.size());

    bool noswap = true;

    std::vector<std::string> history {
        "e4e5", "c2c4", "d3e5"
    };

    bool white_acts = board.white_acts_;

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
    
    unsigned long long fillerlev [8]  = {        
        0x0,
        0xFF,
        0xFFFF,
        0xFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFFFF,
        0xFFFFFFFFFFFF,
        0xFFFFFFFFFFFFFF,        
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
        
        BBoard fill_halfmove_ratio = static_cast<BBoard>(fillerlev[std::min(hrat, 4)]);
        BBoard fill_fullmove_ratio = static_cast<BBoard>(fillerlev[std::min(fmrat, 4)]);

        SetPartialVector(vecflat, fill_halfmove_ratio, noswap, offset);offset+=64;
        SetPartialVector(vecflat, fill_fullmove_ratio, noswap, offset);offset+=64;

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

        auto input = cppflow::tensor(vecflat, {32,8,8});

  /*       for(int i = 0 ; i < 32 ; ++i) {
            print(DebugTensor<float>(input[i]));

        } */

        auto debug_string = DebugTensor<float>(input, 1);
    

        auto expdim_input = cppflow::expand_dims(input, 0);
        auto output = model({{"serving_default_my_input_1:0", expdim_input}},
                            {"StatefulPartitionedCall:0", "StatefulPartitionedCall:1"});     

        auto logits = output[0].get_data<float>();

/* 
        for(auto log : logits) {
            print(log);
        } */       
    


}

void EvaluateMovesInPosition(std::string position)
{
    PerftDividerFactory divider;
    Board b = CREATE(position);
    auto res = divider.Enumerate(b, 1);
    for (const auto r : res)
    {
        print(r.first);
    }
}

void PerformBenchmarkPerft()
{
    std::string startpos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1";
    std::string kiwipep_fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    std::string promotion_fen = "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1";

    auto startposition = CREATE(startpos_fen);
    auto kiwipep = CREATE(kiwipep_fen);
    auto promoboard = CREATE(promotion_fen);

    ChesslibInterface chesslib_io;

    {
        Timer t0;
        const int d = 6;
        // benchmarking
        std::vector<unsigned long long> res = chesslib_io.InitSearch(startposition, SearchType::PERFT, d);

        double t_delta = t0.elapsed();
        std::cout << "search time : " << t_delta << std::endl;

        unsigned long long total = 0;
        for (const auto &dres : res)
        {
            total += dres;
            print(dres);
        }

        std::cout << "Total nodes : " << total << " NPS : " << (unsigned long long)total / t_delta << std::endl;
    }

    {
        const int maxd = 6;
        MGenThreadManager mg_thread_manager;
        Timer t0;
        unsigned long long result = mg_thread_manager.Enumerate(startposition, maxd);
        double delta_t = t0.elapsed();

        std::cout << "Total nodes (threaded) : " << result << " NPS : " << (unsigned long long)result / delta_t << std::endl;
    }
}

void test_threadpooled_mctsexpansion()
{
    srand(time(NULL));

    ZHash::GetInstance(); // trigger init on ZHash singleton

    std::string startpos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string kiwipep_fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

    auto rookup = "1nbqkbn1/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w K - 0 1";
    auto fen3 = "2rq1bk1/1p3ppp/pNn2n2/2Ppr3/1P6/P3B3/1Q2NPPP/R4RK1 w - - 1 19";
    auto fen4 = "5k2/6b1/P2p1p2/1b6/2N5/8/Pq3PPP/4RK1R w - - 2 29";
    auto fen5 = "8/1Pq4B/7k/1KNn4/8/8/8/8 b - - 5 66";
    std::string promotion_fen = "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1";

    Board position = CREATE(startpos_fen);
    OptionsDict params;

    std::unique_ptr<MCTSNodeTree> tree = std::make_unique<MCTSNodeTree>(position);

    tree->MaybeExpandRoot();
    auto threadbranches = tree->DisjointAllBranchesL1();
    ThreadPool tpool;
    tpool.AddJob(tree.get(), threadbranches);
    tree->DebugMetrics();
}

void convert_chessboard_to_neuralplanes(Board board)
{

    auto pbit = LeastBit(board.white_pawn_);

    auto tens = cppflow::fill({8, 8}, board.white_pawn_);
}

void test_networkload(std::string modelpath)
{

    // V0NetworkModel v0m(modelpath);

    cppflow::model model(modelpath);

    auto inputtensor = cppflow::fill({32, 8, 8}, 1.0f);

    auto t1 = cppflow::expand_dims(inputtensor, 0);

    Timer t0;
    for (int i = 0; i < 1; ++i)
    {
        auto output = model({{"serving_default_my_input_1:0", t1}},
                            {"StatefulPartitionedCall:0", "StatefulPartitionedCall:1"});

        auto policy = output[0];
        print(policy);
    }
    print(t0.elapsed());
}

int main()
{
    // test_networkload("../python/train_model_v0_1");

    std::string startpos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    std::string blackpos = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 25 50";

    auto board = CREATE(startpos_fen);
    // convert_chessboard_to_neuralplanes(board);
    
    cppflow::model model("../python/train_model_v0_1");

    PrepareInput(board, model);

    return 0;
}


/*         

        auto input = cppflow::expand_dims(model_input, 0);
        auto output = model({{"serving_default_my_input_1:0", input}},
                            {"StatefulPartitionedCall:0", "StatefulPartitionedCall:1"});     */