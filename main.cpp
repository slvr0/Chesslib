#include <iostream>

#include <vector>
#include <random>

#include "src/blowfish/chessboard_generator.h"
#include "src/blowfish/chessboard_extractor.h"
#include "src/blowfish/move_generator.h"
#include "src/blowfish/position_meta_data.h"
#include "src/blowfish/perft_divider_autotraverse.h"
#include "src/chess_interface/movegen_interface.h"

#define CREATE(string) ChessboardGenerator::CreateFromFen(string)

int main() {

    std::string startpos_fen    = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string kiwipep_fen     = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    std::string promotion_fen   = "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1";
    
    ChesslibInterface chesslib_io;

    auto startposition  = CREATE(startpos_fen);
    auto kiwipep        = CREATE(kiwipep_fen);
    auto promoboard     = CREATE(promotion_fen);

    Timer t0;
    const int d = 6;
    //benchmarking
    std::vector<unsigned long long> res = chesslib_io.InitSearch(startposition, SearchType::PERFT, d);

    std::cout << "search time : " << t0.elapsed() << std::endl;

    for(const auto & dres : res) {
        print(dres);
    }


    

    return 0;
}



