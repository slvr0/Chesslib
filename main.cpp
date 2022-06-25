#include <iostream>

#include "src/core2/chess_board.h"
#include "src/core2/attack_tables.h"
#include "src/core2/attack_tables_handler.h"
#include "src/core2/move_generator.h"

#include "src/core2/global_utils.cpp"
#include "src/core2/attack_tables.h"

#include <vector>

void getAllOnes(uint64_t v1) {

    auto popcount = pop_count(v1);
    for(int i = 0 ; i < popcount; ++ i) {
        uint8_t lb = least_bit(v1);

        print((int)lb);

        //this algorithm works up until 32 bits, figure out how to make it work for 64
        v1 &= -1 << lb + 1;

    }
}

bool Chesslib::AttackTablesHandler::initialized = false;

int main() {

  
    //test_correctply();

    //Chesslib::Board b1 {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};

    //std::cout << (int)b1.getState().castle << std::endl;    
     
    


/*     Chesslib::BoardConsoleGUI::printConsole(b1);

    Chesslib::AttackTablesHandler attack_tables;

    Chesslib::U64 pattern = attack_tables.getBishopAttackPattern(b1.all(), 0);     */

    std::string pos1 = "2k5/2r5/8/2R2b2/8/3B4/2K5/8 w - - 0 1";
    std::string pin_issue = "2k5/2r5/8/2R2b2/8/3B4/1pK5/8 w - - 0 1";
    Chesslib::Board b1{pin_issue};

    Chesslib::BinaryMoveGenerator movegenerator;   


    Chesslib::Transitions transitions = movegenerator.getMoves(b1);
    Chesslib::BoardConsoleGUI::translateTransitions(b1, transitions);

    Timer t0;    

    /* 
    Chesslib::BoardConsoleGUI::translateTransitions(b1, transitions); */

    for(int i = 0 ; i < 1000000; ++i) {
        Chesslib::Transitions transitions = movegenerator.getMoves(b1);    
    }  

    print(t0.elapsed());


    return 0;
}
