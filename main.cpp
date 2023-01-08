#include <iostream>
#include <string_view>
#include <vector>
#include <random>

#include "src/blowfish/chessboard_generator.h"
#include "src/blowfish/chessboard_extractor.h"
#include "src/blowfish/move_generator.h"
#include "src/blowfish/position_meta_data.h"

#include "src/chess_interface/movegen_interface.h"

int main() {

    std::string basic_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
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



    Board chessboard = ChessboardGenerator::CreateFromFen(basic_pos);    
    //Board chessboard = ChessboardGenerator::CreateFromFen(pin_test_02);   

/*     std::cout << PrintBoardsAndMask(0,0, chessboard, chessboard) << std::endl; */

  
    

 //performance check
/*      {   
        Timer t0; 
        for(int i = 0; i < 1e6 ; ++i) {
            white_movegen.GetPseudoLegalMoves(chessboard);
        }

        print(t0.elapsed());
    }  */
    int d = 1;

    ChesslibInterface chesslib_io;

    BoardDummy dummy;
    {   
        Timer t0; 
        int from = 3;
        int to = 4;
        for(int i = 0; i < 1e6 ; ++i) {
            chesslib_io.InitSearch(chessboard, SearchType::PERFT, 2);
            /* int from = 3;
            int to = 4; */


/*                 for(int i = 0 ; i < 20 ; ++i) {
                    Board nb = UpdateMove(chessboard, MoveTypes::PawnCapture, PieceType::PAWN, from, to); 
                } */
        }

        print(t0.elapsed());
    }  



  /*   BBoard rookmove = 1ULL << 56 | 1ULL << 59;
    BBoard kingmove = 1ULL << 60 | 1ULL << 58;
    print(rookmove);
    print(kingmove); */

    return 0;
}

