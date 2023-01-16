#include "gtest/gtest.h"

//#include "test_board.h"
//#include "test_attack_tables.h"
#include "test_move_generator.h"

bool Chesslib::AttackTablesHandler::initialized = false;

GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from gtest_main.cc\n");

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


//legacy
#add_library(chesslib
#    core2/attack_tables.h
#    core2/attack_tables.cpp    
#    core2/chess_board.h
#    core2/chess_board.cpp
#    core2/attack_tables_handler.h
#    core2/attack_tables_handler.cpp
#    core2/move_generator.h
#    core2/move_generator.cpp
#    core2/global_utils.cpp
#)


#add_library(chesslib2
#    core3/chesslib_def.h
#    core3/chesslib_movecache.h
#    core3/movegen_interface.h
#    core3/movegen_interface.cpp
#    core3/chesslib_scenarios.h
#    core3/chesslib_generate_metadata.h
#    core3/chesslib_fenreader.h
#    core3/chesslib_boardinfo.h
#    core3/chesslib_movegen.h
#    core3/chesslib_callback.h
#) 



//Prev main 

/* #include "src/core2/chess_board.h"
#include "src/core2/attack_tables.h"
#include "src/core2/attack_tables_handler.h"
#include "src/core2/move_generator.h"

#include "src/core2/attack_tables.h"
 */

#include "src/core2/global_utils.cpp"

#include "src/core3/chesslib_movecache.h"
#include "src/core3/chesslib_def.h"
#include "src/core3/chesslib_board.h"
#include "src/core3/chesslib_generate_metadata.h"
#include "src/core3/chesslib_movegen.h"

//bool Chesslib::AttackTablesHandler::initialized = false;


//Chesslib::BinaryMoveGenerator gen;
/* 
int traverse_tree(Chesslib::Board& position, int & depth, const int & maxdepth, int & count, double & processboard_time) {   
      

    if(depth == maxdepth) return ++count;

    auto nmoves = gen.getMoves(position);  
    for(auto & move : nmoves) {

        if(move.instr == 'E') print("Enp!");
        
                    
        Chesslib::Board cp_board = position;

        //Timer t0;
        cp_board.applyMove(move);
   
        //processboard_time += t0.elapsed();

        int new_depth = depth + 1;
        traverse_tree(cp_board, new_depth, maxdepth, count, processboard_time);
    } 
    
    return count;
}
void Perfqt(std::string startpos, const int & depth) {
    
    std::string startposs = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Chesslib::Board starting_board(startposs);

    int ply_start = 0;
    int count = 0 ;
    double processboard_time = 0.0;

    Timer tot;
    int res = traverse_tree(starting_board, ply_start, depth, count, processboard_time);

    double tot_elapsed = tot.elapsed();

    print(res);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
} */

int main() {
    /* std::string origin = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string origin_b = "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1";
    std::string pos1 = "2k5/2r5/8/2R2b2/8/3B4/2K5/8 w - a2 0 1";
    std::string pin_issue = "2k5/2r5/8/2R2b2/8/3B4/1pK5/8 w - - 0 1";
    std::string trickiest_enp = "8/8/8/8/1Q1Pp1k1/8/8/3K4 b - - 0 1";
    std::string clear_promo = "8/3P4/8/8/4p1k1/8/8/3K4 w - - 0 1";
    std::string intersect_promo = "2K1r3/3P4/4bn2/8/4p1k1/8/8/8 w - - 0 1";
    std::string easiest_castle = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 1";
    std::string castle_attacked = "rnbqk1nr/pppppppp/8/8/2b5/8/PPPP1PPP/RNBQK2R w KQkq - 0 1";
    std::string castle_occ = "rnbqk1nr/pppppppp/8/8/8/8/PPPP1PPP/RNBQK1bR w KQkq - 0 1";
    std::string standard_enp = "3k4/8/8/2PpP3/3K4/8/8/8 w - d6 0 2";
    std::string tricky_enp_1 = "3k4/8/8/2PpP3/4K3/8/8/8 w - d6 0 2";
    std::string tricky_enp_2 = "8/8/8/1k6/3Pp3/8/8/4KQ2 b - d3 0 1";
    std::string tricky_enp_3 = "8/8/8/8/1k1Pp2Q/8/8/4K3 b - d3 0 1";
    std::string tricky_enp_4 = "8/8/8/8/1Q1Pp2k/8/8/4K3 b - d3 0 1";
    std::string tricky_enp_5 = "3k4/4r3/8/2PpP3/4K3/8/8/8 w - d6 0 2";
    std::string strange_position_1 = "3k4/3r3b/b2N4/4pP2/2P5/3K4/8/8 w - - 0 3";
    std::string std_pos = "rnbqkbnr/ppp1pppp/3p4/7P/8/8/PPPPPPP1/RNBQKBNR b KQkq - 0 2";
    std::string std_pos1 = "rnbqkbnr/pppp1ppp/8/4p3/3P1P2/8/PPP1P1PP/RNBQKBNR b KQkq - 0 2";
    
    Chesslib::Board b1{std_pos1};    
    Chesslib::BinaryMoveGenerator movegenerator;   

    {
        srand (time(NULL));
        for(int i = 0 ; i < 2 ;  ++i) {
            auto transitions = movegenerator.getMoves(b1);   
            int v = rand() % (transitions.size() - 1);
            b1.applyMove(transitions.at(v));

            print("legal moves : ");
            Chesslib::BoardConsoleGUI::translateTransitions(b1, transitions);

            std::cout << " took action : " << Chesslib::BoardConsoleGUI::translateTransition(b1, transitions.at(v)) << " || new state : " << std:: endl;

            auto state = b1.getState();
            std::cout << "white castle kingside : "<< state.white_oo << std::endl;
            std::cout << "white castle queenside : "<< state.white_ooo << std::endl;
            std::cout << "black castle kingside : "<< state.black_oo << std::endl;
            std::cout << "black castle kingside : "<< state.black_ooo << std::endl;
            std::cout << "enpassant : "<< state.enp << std::endl;     

            Chesslib::BoardConsoleGUI::printConsole(b1);
        } 
    }   */
  
    //Perfqt(origin, 4);

    constexpr uint64_t someval = 0xFF13ABull;

    Timer t0;

    constexpr std::string_view tricky_enp_4 = "8/8/8/8/1Q1Pp2k/8/8/4K3 b - d3 0 1";
    constexpr std::string_view std_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Chesslib2::Board b1(std_pos);

    //print(Chesslib2::BoardInfo::_map(0,1, b1, b1));

/*     print(b1.state_.white_acts_);
    print(b1.state_.white_oo_);
    print(b1.state_.white_ooo_);
    print(b1.state_.black_oo_);
    print(b1.state_.black_ooo_); */

    Chesslib2::MoveGenerator movegen;
    movegen.SearchPseudoMoves(b1);



    print(t0.elapsed());
