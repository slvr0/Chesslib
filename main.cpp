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

      
    //Board chessboard = ChessboardGenerator::CreateFromFen(pin_test_02);   

/*     std::cout << PrintBoardsAndMask(0,0, chessboard, chessboard) << std::endl; */

    ChesslibInterface chesslib_io;

    
/* 

    {   
        Timer t0; 
        int from = 3;
        int to = 4;
        for(int i = 0; i < 1e6 ; ++i) {
                  
   
        }

        print(t0.elapsed());
    }  


 */

    Board fixme = ChessboardGenerator::CreateFromFen("rnbqkbnr/pppp1ppp/8/8/3Pp1P1/5P2/PPP1P2P/RNBQKBNR b KQkq d3 0 3");
    Board kiwipep = ChessboardGenerator::CreateFromFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    Board promotionbugs = ChessboardGenerator::CreateFromFen("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1");
    
    Board chesstest = ChessboardGenerator::CreateFromFen("rnbqkb1r/pppppppp/5n2/8/5P2/8/PPPPPKPP/RNBQ1BNR b kq - 2 2");
    

    Board startpos = ChessboardGenerator::CreateFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");  
    Board perftdivide_follows1 = ChessboardGenerator::CreateFromFen("rnbqkbnr/pppppppp/8/8/6P1/8/PPPPPP1P/RNBQKBNR b KQkq - 0 1");
    Board perftdivide_follows2 = ChessboardGenerator::CreateFromFen("rnbqkbnr/1ppppppp/p7/8/6P1/8/PPPPPP1P/RNBQKBNR w KQkq - 0 2");
    Board perftdivide_follows3 = ChessboardGenerator::CreateFromFen("rnbqkbnr/1ppppppp/p7/6P1/8/8/PPPPPP1P/RNBQKBNR b KQkq - 0 2");

    Board perft_new3 = ChessboardGenerator::CreateFromFen("rnbqkbnr/pppp1ppp/8/4p3/7P/5P2/PPPPP1P1/RNBQKBNR b KQkq h5 0 2");
    Board any_test1 = ChessboardGenerator::CreateFromFen("rnbqkbnr/1pppppp1/p7/6Pp/8/8/PPPPPP1P/RNBQKBNR w KQkq h6 0 3");
    Board any_test2 = ChessboardGenerator::CreateFromFen("rnbqkbnr/1pppp1pp/p7/5pP1/8/8/PPPPPP1P/RNBQKBNR w KQkq f6 0 3");


    auto l1 = ChessboardGenerator::CreateFromFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    auto l3 = ChessboardGenerator::CreateFromFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    auto l4 = ChessboardGenerator::CreateFromFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    auto fin = ChessboardGenerator::CreateFromFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    //auto ret = chesslib_io.InitSearch(any_test1, SearchType::PERFT_DIVIDE, 1);  

    //auto ret2 = chesslib_io.InitSearch(l1, SearchType::PERFT_DIVIDE, 2); 

    PerftDividerAutotraverse perft_autotr; 
    perft_autotr.EnumerateDivide(kiwipep, 2);


    //std::cout << exec("stockfish position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 go perft 2") << std::endl; // this works fine
/*     std::cout << system("stockfish go depth 2"); */

/*     system("position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    system("go perft 2");
    system("quit"); */

    /*
    Black Positions

    //BP1 = 7k/pp1b1r2/2p5/3p3p/5Ppq/P2BQ2N/1P4P1/5R1K b - - 0 1
    //BP2 = 1q6/1P4R1/4Nn1k/8/2K5/5rP1/2B5/8 b - - 0 1
    //BP3 = 4R3/p3k1p1/1P1N3p/P3b3/8/5P1P/6P1/5K2 b - - 2 55
    //BP4 = 2k4r/1p3Bpp/p4n2/4p3/4P3/5PKP/PPn5/1NR5 b - - 1 22
    //BP5 = r2q1rk1/4bpp1/p1b1pn1p/1pPpB3/1P1P4/P1NB3P/2Q2PP1/4RRK1 b - - 2 17
    //BP6 = 8/8/2kpb3/8/3NB3/4K3/8/8 b - - 2 55
    //BP7 = 8/8/2k5/8/8/K7/p1p3p1/2RN2N1 b - - 2 55
    //BP8 = n6n/R1pk1P2/3pp2q/5Pb1/8/8/6K1/n6n b - - 2 55
    //BP9 = 8/8/8/1k6/1pP5/8/1R6/4K3 b - c3 0 55
    //BP10 = 8/8/8/8/kpP1RR2/8/8/4K3 b - - c3 55
    //BP11 = 4Q3/3r4/2k3nR/3p4/8/5B2/2pNN3/4K3 b - - 2 55
    
    */



    return 0;
}



