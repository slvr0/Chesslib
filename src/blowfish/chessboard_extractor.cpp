#include "chessboard_extractor.h"

char AtBoardPosition(const Square& square, const Board& brd) {  

    if      ( square &brd.white_pawn_)     return 'P';
    else if ( square &brd.white_knight_)   return 'N';
    else if ( square &brd.white_bishop_)   return 'B';
    else if ( square &brd.white_rook_)     return 'R';
    else if ( square &brd.white_queen_)    return 'Q';
    else if ( square &brd.white_king_)     return 'K';
    else if ( square &brd.black_pawn_)     return 'p';
    else if ( square &brd.black_knight_)   return 'n';
    else if ( square &brd.black_bishop_)   return 'b';
    else if ( square &brd.black_rook_)     return 'r';
    else if ( square &brd.black_queen_)    return 'q';
    else if ( square &brd.black_king_)     return 'k';

    else return '.';
}


std::string PrintBoardsAndMask(uint64_t val1, uint64_t val2, const Board& val3, const Board& val4) {

    std::string str(64 * 4 + 4 * 8, 'o'); //2 boards with space between

    for (uint64_t i = 0, c = 0; i < 64; i++)
    {
        uint64_t bitmask = (1ull << 63) >> i;

        if ((bitmask & val1) != 0) str[c] = 'X';
        else str[c] = '.';

        if ((bitmask & val2) != 0) str[c + 9] = 'X';
        else str[c + 9] = '.';

        str[c + 18] = AtBoardPosition(bitmask, val3);
        str[c + 27] = AtBoardPosition(bitmask, val4);

        c++;

        if ((i + 1) % 8 == 0) {
            str[c] = ' ';
            str[c + 9] = ' ';
            str[c + 18] = ' ';
            str[c + 27] = '\n';
            c += 28;
        }
    }
    return str;
}

void BoardConsoleGUI::PrintBoard(const Board & board, const int & print_mode, char spec_type) {

    auto printer = BoardConsoleGUI::CopyBoardContent(board);
    int idx = 56;

    print("___A__B__C__D__E__F__G__H__");

    std::cout << int(idx/8) + 1;

    while( idx >= 0)
    {
    if ((idx + 1) % 8 == 0)
    {
    std::cout << " |" << printer.at(idx) << " |" << std::endl;
    idx -= 15;
    if (int(idx / 8) + 1 > 0)
    {
        std::cout << (int(idx / 8) + 1);
    }
    }
    else
    {
    std::cout << " |" << printer.at(idx);
    idx += 1;
    }
    }   
    print("___A__B__C__D__E__F__G__H__");
    print("\n");
}

void BoardConsoleGUI::PrintBoard(unsigned long const & board) {
    std::vector<char> printer;

    for (int i = 0 ; i<64 ; ++i)
    {
        if((1ULL << i) & board)  printer.emplace_back('X');
        else printer.emplace_back('.');
    }

    int idx = 56;

    print("___A__B__C__D__E__F__G__H__");

    std::cout << int(idx/8) + 1;

    while( idx >= 0)
    {
    if ((idx + 1) % 8 == 0)
    {
    std::cout << " |" << printer.at(idx) << " |" << std::endl;
    idx -= 15;
    if (int(idx / 8) + 1 > 0)
    {
        std::cout << (int(idx / 8) + 1);
    }
    }
    else
    {
    std::cout << " |" << printer.at(idx);
    idx += 1;
    }
    }   
    print("___A__B__C__D__E__F__G__H__");
    print("\n");
}

std::vector<char> BoardConsoleGUI::CopyBoardContent(const Board & board){

    std::vector<char> printer;
    for (int i = 0 ; i<64 ; ++i)
    {
        auto spec_piece = AtBoardPosition(1ULL << i, board);      
        printer.emplace_back(spec_piece);
    }
    return printer;
}
