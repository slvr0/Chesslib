#include <iostream>
#include <string_view>
#include <vector>
#include <random>

#include "src/blowfish/chessboard_generator.h"
#include "src/blowfish/chessboard_extractor.h"
#include "src/blowfish/move_generator.h"
#include "src/blowfish/position_meta_data.h"
#include "src/blowfish/perft_divider_autotraverse.h"
#include "src/chess_interface/movegen_interface.h"

int main() {
    std::string opendiag = "rnbqkbnr/pp2pppp/2p5/3p4/3P4/6P1/PPP1PP1P/RNBQKBNR w KQkq - 0 3";
    std::string open_diag_rooks = "rnbqkbnr/p4p1p/4p3/1Ppp2p1/3P3P/5NP1/1PP1PP2/RNBQKB1R w KQkq - 0 7";

    std::string checkpin_pos_00 = "1k6/8/8/8/8/3K2r1/8/8 w - - 0 1";
    std::string checkpin_pos_01 = "1k6/8/8/5b2/8/3KP1r1/8/8 w - - 0 1";
    std::string pin_test_00 = "2k5/2p4b/8/8/4Q3/3K4/8/8 w - - 0 1";
    std::string pin_test_01 = "2k5/2p4b/8/8/2b1Q3/3K4/8/8 w - - 0 1"; 
    std::string pin_test_02 = "2k5/2p4b/8/1p6/2b1Q3/3K4/8/8 w - - 0 1";

    std::string pawn_movement_test_01 = "1k6/ppp1P2p/8/2p3P1/3P1p2/1K6/1P3P2/8 w - - 0 1"; //assert correct!
    std::string pawn_movement_test_02 = "1k6/ppp1P3/8/2p3Pp/3P1p2/1K6/1P3P2/8 w - h6 0 2"; //assert correct!
    std::string pawn_movement_test_022 = "1k4nn/ppp1P1Pp/8/2p2pP1/3P1p2/1K6/1P3P2/8 w - f6 0 2"; //assert correct!
    std::string pawn_movement_test_03 = "1k4bn/ppp1P1Pp/8/2p3P1/3P1p2/1K6/1P3P2/8 w - - 0 1"; //pawn asserts correct, king moves absence?

    std::string pawn_movement_test_04 = "1k4nn/ppp1P1Pp/8/2p3P1/3P1p2/1K6/1P3P2/8 w - - 0 11"; //assert correct!    
    std::string pawn_movement_test_05 = "1k2b1nn/ppp2PPp/8/2p3P1/K2P1p2/8/1P3P2/8 w - - 0 1"; //assert correct!

    std::string pawn_difficult_enp_01 = "5k2/8/8/1K1Pp1q1/8/8/8/8 w - e6 0 2";

    std::string pawn_difficult_enp_02 = "5k2/8/8/1K1pP1q1/8/8/8/8 w - d6 0 2";

    std::string castle_01 = "2k4r/8/8/8/8/8/4PPP1/R3K2R w KQk - 0 1";

    Board fixme = ChessboardGenerator::CreateFromFen("rnbqkbnr/pppp1ppp/8/8/3Pp1P1/5P2/PPP1P2P/RNBQKBNR b KQkq d3 0 3");
    Board kiwipep = ChessboardGenerator::CreateFromFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    Board promotionbugs = ChessboardGenerator::CreateFromFen("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1");
    
    Board chesstest = ChessboardGenerator::CreateFromFen("rnbqkb1r/pppppppp/5n2/8/5P2/8/PPPPPKPP/RNBQ1BNR b kq - 2 2");
    

    Board startpos = ChessboardGenerator::CreateFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");  
    Board perftdivide_follows1 = ChessboardGenerator::CreateFromFen("rnbqkbnr/pppppppp/8/8/6P1/8/PPPPPP1P/RNBQKBNR b KQkq - 0 1");
    Board perftdivide_follows2 = ChessboardGenerator::CreateFromFen("rnbqkbnr/1ppppppp/p7/8/6P1/8/PPPPPP1P/RNBQKBNR w KQkq - 0 2");
    Board perftdivide_follows3 = ChessboardGenerator::CreateFromFen("rnbqkbnr/1ppppppp/p7/6P1/8/8/PPPPPP1P/RNBQKBNR b KQkq - 0 2");

    Board perft_new3 = ChessboardGenerator::CreateFromFen("rnbqkbnr/pppp1ppp/8/4p3/7P/5P2/PPPPP1P1/RNBQKBNR b KQkq h5 0 2");
    
    Board any_test2 = ChessboardGenerator::CreateFromFen("rnbqkbnr/1pppp1pp/p7/5pP1/8/8/PPPPPP1P/RNBQKBNR w KQkq f6 0 3");  

    ChesslibInterface chesslib_io;

    Board any = ChessboardGenerator::CreateFromFen("5q2/1NP5/8/3Pp1rk/3K4/8/8/8 w - e6 0 1");

    

/*     
    {   //rigid autotraverse testscoping
        PerftDividerAutotraverse perft_autotr; 
        perft_autotr.EnumerateDivide(promotionbugs, 4) ;   

        //add this to debug certain position closer
        auto problem = ChessboardGenerator::CreateFromFen("r3k2N/p2pq1b1/1n2pnp1/1bpP4/1p2P3/2N2Q1p/PPPBBPPP/1R2K2R b Kq - 0 2");

        PerftDividerFactory perftdivider;
        std::map<std::string, BoardNode*> res = perftdivider.Enumerate(problem, 1);

        for(auto & v : res) {
            std::cout << v.first << ": " << v.second->GetSubnodes() << "\t" << BoardAsFen(v.second->board_) << std::endl;
        }
        print(res.size());  
    }
 */

/*     
{
    const int maxd = 6;
    PerftDividerFactory perftdivider;
    std::map<std::string, BoardNode*> res = perftdivider.Enumerate(startpos, 1);

    for(auto & v : res) {
        //std::cout << v.first << ": " << v.second->GetSubnodes() << "\t" << BoardAsFen(v.second->board_) << std::endl;
        PerftDividerAutotraverse perft_autotr; 
        perft_autotr.EnumerateDivide(v.second->board_, maxd - 1);
    }
}
 */

/* 
{ 
    std::vector<std::string> erroneous_positions {   
        "5q2/4p3/8/2KP4/8/3k4/8/8 b - - 0 1",
        "8/8/8/4k3/KR1Pp3/8/5pn1/2Q5 b - d3 0 1" 
        "5q2/1NP5/8/3Pp1rk/3K4/8/8/8 w - e6 0 1", 
        "rnbqkbnr/pppppppp/8/8/1P6/8/P1PPPPPP/RNBQKBNR b KQkq b3 0 1",
        "rnbqkbnr/pppppppp/8/8/2P5/8/PP1PPPPP/RNBQKBNR b KQkq c3 0 1",
        "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 1"
    };

    for(const auto & err : erroneous_positions) {

        Board b = ChessboardGenerator::CreateFromFen(err);

        PerftDividerAutotraverse perft_autotr;         
        perft_autotr.EnumerateDivide(b, 4);  

        PerftDividerFactory perftdivider;

        std::map<std::string, BoardNode*> res = perftdivider.Enumerate(b, 1);  

        for(auto & v : res) {
            std::cout << v.first << ": " << v.second->GetSubnodes() << "\t" << BoardAsFen(v.second->board_) << std::endl;
        }  
        exit(1);             
        
    }

}
 */







    return 0;
}



