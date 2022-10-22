#pragma once 
#include "gtest/gtest.h"

#include "../src/core2/move_generator.h"


Chesslib::BinaryMoveGenerator gen;

int traverse_tree(Chesslib::Board& position, int & depth, const int & maxdepth, int & count, double & processboard_time) {   
      

    if(depth == maxdepth) return ++count;

    auto nmoves = gen.getMoves(position);         

    for(auto & move : nmoves) {
                    
        Chesslib::Board cp_board = position;

        Timer t0;
        cp_board.applyMove(move);
   
        processboard_time += t0.elapsed();

        int new_depth = depth + 1;
        traverse_tree(cp_board, new_depth, maxdepth, count, processboard_time);
    } 
    
    return count;
}
void Perfqt(const int & depth) {
    
    std::string startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Chesslib::Board starting_board(startpos);

    int ply_start = 0;
    int count = 0 ;
    double processboard_time = 0.0;

    Timer tot;
    int res = traverse_tree(starting_board, ply_start, depth, count, processboard_time);

    double tot_elapsed = tot.elapsed();

    print(res);
    print(processboard_time);
    print(tot_elapsed);
}

TEST(ChessBinaryMoveGenerator, BinaryMoveGenCorrectPerfQt) {
    Perfqt(1);
}
    
