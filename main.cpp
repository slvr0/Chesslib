#include <iostream>

#include <vector>
#include <random>
#include <cstdlib>

#include "src/blowfish/chessboard_generator.h"
#include "src/blowfish/chessboard_extractor.h"
#include "src/blowfish/move_generator.h"
#include "src/blowfish/position_meta_data.h"
#include "src/blowfish/perft_divider_autotraverse.h"
#include "src/blowfish/perft_mg_thread.h"

#include "src/chess_interface/movegen_interface.h"


#include "src/mcts_new/mcts_simulation_env.h"
#include "src/mcts_new/mcts_jobqueue.h"

void VisualizeUCB(int range) {

    for(int i = 1; i < range; ++i) {
        if(i % 100 == 0) {
            print(std::sqrt((std::log(20*i) / i)));
        }
    }    
}

void EvaluateMovesInPosition(std::string position) {        
    PerftDividerFactory divider;
    Board b = CREATE(position);
    auto res = divider.Enumerate(b,1);
    for(const auto r : res) {
        print(r.first);
    }
}

void PerformBenchmarkPerft() {
    std::string startpos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1";
    std::string kiwipep_fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    std::string promotion_fen = "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1";

    auto startposition  = CREATE(startpos_fen);
    auto kiwipep        = CREATE(kiwipep_fen);
    auto promoboard     = CREATE(promotion_fen);
    
    ChesslibInterface chesslib_io;

    {
        Timer t0;
        const int d = 6;
        //benchmarking
        std::vector<unsigned long long> res = chesslib_io.InitSearch(startposition, SearchType::PERFT, d);

        double t_delta = t0.elapsed();
        std::cout << "search time : " << t_delta << std::endl;

        unsigned long long total = 0;
        for(const auto & dres : res) {
            total += dres;
            print(dres);
        }

        std::cout << "Total nodes : " << total << " NPS : " << (unsigned long long) total/t_delta << std::endl;
    }

    {
        const int maxd = 6;
        MGenThreadManager mg_thread_manager;
        Timer t0;
        unsigned long long result = mg_thread_manager.Enumerate(startposition, maxd);
        double delta_t = t0.elapsed();

        std::cout << "Total nodes (threaded) : " << result << " NPS : " << (unsigned long long) result/delta_t << std::endl;
    }     
}

int main()
{
    srand (time(NULL));

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

    std::unique_ptr<MCTSNodeTree> tree = std::make_unique<MCTSNodeTree> (position);

    tree->MaybeExpandRoot();
    auto threadbranches = tree->DisjointAllBranchesL1();    
    ThreadPool tpool;    
    tpool.AddJob(tree.get(), threadbranches);
    tree->DebugMetrics(); 



    return 0;
}
