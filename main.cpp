#include <iostream>
#include "src/ply_test.h"

#include "src/core2/chess_board.h"
#include "src/core2/attack_tables.h"
#include "src/core2/attack_tables_handler.h"


bool Chesslib::AttackTablesHandler::initialized = false;

int main() {

  
    //test_correctply();

    Chesslib::Board b1 {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};

    //std::cout << (int)b1.getState().castle << std::endl;

    
    

    Chesslib::AttackTablesHandler attack_tables;     

   
    

    
    
    return 0;
}
