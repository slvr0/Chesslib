#include <iostream>
#include <string_view>
#include <vector>
#include <random>

#include "src/blowfish/chessboard_generator.h"
#include "src/blowfish/chessboard_extractor.h"
#include "src/blowfish/move_generator.h"
#include "src/blowfish/position_meta_data.h"

int main() {

    std::string basic_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string opendiag = "rnbqkbnr/pp2pppp/2p5/3p4/3P4/6P1/PPP1PP1P/RNBQKBNR w KQkq - 0 3";
    std::string open_diag_rooks = "rnbqkbnr/p4p1p/4p3/1Ppp2p1/3P3P/5NP1/1PP1PP2/RNBQKB1R w KQkq - 0 7";

    std::string checkpin_pos_00 = "1k6/8/8/8/8/3K2r1/8/8 w - - 0 1";
    std::string checkpin_pos_01 = "1k6/8/8/5b2/8/3KP1r1/8/8 w - - 0 1";

    Board chessboard = ChessboardGenerator::CreateFromFen(checkpin_pos_01);    

/*     std::cout << PrintBoardsAndMask(0,0, chessboard, chessboard) << std::endl; */

    WhiteMoveGenerator white_movegen;

/*  //performance check
    {   
        Timer t0; 
        for(int i = 0; i < 1e6 ; ++i) {
            white_movegen.GetPseudoLegalMoves(chessboard);
        }

        print(t0.elapsed());
    }
 */

    white_movegen.GetPseudoLegalMoves(chessboard);

    return 0;
}

 //std::cout << PrintBoardsAndMask(to,to,board,board) << std::endl;