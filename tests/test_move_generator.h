#pragma once 

/* MoveGenerator gen;
int traverse_tree(ChessBoard& position, int & depth, const int & maxdepth, int & count, double & processboard_time) {   
      

    if(depth == maxdepth) return ++count;

    auto nmoves = gen.get_legal_moves(position).first;           

    for(auto & move : nmoves.get_moves()) {
                    
        ChessBoard cp_board = position;

        Timer t0;
        cp_board.update_from_move(move);
        cp_board.mirror();
        processboard_time += t0.elapsed();

        int new_depth = depth + 1;
        traverse_tree(cp_board, new_depth, maxdepth, count, processboard_time);
    } 
    
    return count;
}
void test_correctply() {
    init_tables();

    ChessBoard starting_board(startpos);

    int maxply = 4;
    int ply_start = 0;
    int count = 0 ;
    double processboard_time = 0.0;

    Timer tot;
    int res = traverse_tree(starting_board, ply_start, maxply, count, processboard_time);

    double tot_elapsed = tot.elapsed();

    print(res);
    print(processboard_time);
    print(tot_elapsed);
}

 */