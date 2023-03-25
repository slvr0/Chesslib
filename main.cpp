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
#include "src/network/v0input_parser.h"
#include "src/network/network_manager.h"

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

//next goal : 
/*
A NodeGraph can be passed for evaluation.
Its first expanded, gets hashed, sets historic knowledge.

Initially checks its status, if its terminal directly backpropagates status. 
Then sent for eval, to :: NodeGraphManager, this has control over 


*/

int main()
{
    // test_networkload("../python/train_model_v0_1");
    std::string startpos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string blackpos = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 25 50";
    std::string enp_pos_black = "rnbq1rk1/pp2bpp1/2p1pn2/P2pP3/2PP2Pp/2N5/1P2QP1P/R1B1KBNR b KQ g3 0 9";

    auto board = CREATE(enp_pos_black);
    // convert_chessboard_to_neuralplanes(board);    

    NodeGraph* graph = new NodeGraph(board, NodeInfo(0,0,""), nullptr);


    std::unique_ptr<MCTSNodeInserter> inserter = std::make_unique<MCTSNodeInserter> ();
    MCTSNodeExpansionHeader nodexpander(inserter.get());
    nodexpander.ExpandNodeFull(graph, true);    

    NetworkManager network_manager;
    network_manager.LoadNetwork("../python/train_model_v0_1");
    

    network_manager.Parse(graph);

    for(auto & edge :  graph->GetEdges()) {
        edge->DebugDisplay(0, false);
    }


    return 0;
}


/*         

        auto input = cppflow::expand_dims(model_input, 0);
        auto output = model({{"serving_default_my_input_1:0", input}},
                            {"StatefulPartitionedCall:0", "StatefulPartitionedCall:1"});     */