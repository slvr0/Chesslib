#pragma once
#include <iostream>
#include <string_view>
#include "chessboard.h"
#include "defs.h"



struct Board;

class ChessboardGenerator {

public :     
    static Board CreateFromFen(const std::string& fen); 

};

#define CREATE(string) ChessboardGenerator::CreateFromFen(string)


