#include <iostream>
#include <string_view>
#include <vector>
#include <random>

#include "src/blowfish/chessboard_generator.h"
#include "src/blowfish/chessboard_extractor.h"
#include "src/blowfish/move_generator.h"

int main() {

    std::string basic_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string opendiag = "rnbqkbnr/ppp1pppp/8/3p4/3P4/8/PPP1PPPP/RNBQKBNR w KQkq - 0 2";

    Board chessboard = ChessboardGenerator::CreateFromFen(opendiag);    

/*     std::cout << PrintBoardsAndMask(0,0, chessboard, chessboard) << std::endl; */

    WhiteMoveGenerator white_movegen;
    white_movegen.GetPseudoLegalMoves(chessboard);

    return 0;
}

 //std::cout << PrintBoardsAndMask(to,to,board,board) << std::endl;