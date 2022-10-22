#pragma once

#include <iostream>
#include "chesslib_def.h"
#include "chesslib_fenreader.h"

namespace Chesslib2 {

struct BoardState {
    const bool white_acts_ = true;
    
    const bool white_oo_ = true;
    const bool white_ooo_ = true;
    const bool black_oo_ = true;
    const bool black_ooo_ = true;

    const bool has_ep_ = false;

    BoardState() {}

    BoardState(const BoardState& state) :
        white_acts_(state.white_acts_), 
        white_oo_(state.white_oo_), white_ooo_(state.white_ooo_), black_oo_(state.black_oo_), black_ooo_(state.black_ooo_),        
        has_ep_(state.has_ep_)
    {

    }

    BoardState(std::string_view fen_string) : 
        white_acts_(FEN::FenInfo<FenField::white>(fen_string)),
        white_oo_(FEN::FenInfo<FenField::WCastleR>(fen_string)), white_ooo_(FEN::FenInfo<FenField::WCastleL>(fen_string)), 
        black_oo_(FEN::FenInfo<FenField::BCastleR>(fen_string)), black_ooo_(FEN::FenInfo<FenField::BCastleL>(fen_string)), 
        has_ep_(FEN::FenInfo<FenField::hasEP>(fen_string))   
    {

    }

};

struct Board {
    const BBoard white_pawn_;
    const BBoard white_knight_;
    const BBoard white_bishop_;
    const BBoard white_rook_;
    const BBoard white_queen_;
    const BBoard white_king_;

    const BBoard black_pawn_;
    const BBoard black_knight_;
    const BBoard black_bishop_;
    const BBoard black_rook_;
    const BBoard black_queen_;
    const BBoard black_king_;    

    const BBoard white_;
    const BBoard black_;    
    const BBoard occ_;

    const BoardState state_;

    Board(
    BBoard white_pawn, BBoard white_knight, BBoard white_bishop, BBoard white_rook, BBoard white_queen, BBoard white_king,
    BBoard black_pawn, BBoard black_knight, BBoard black_bishop, BBoard black_rook, BBoard black_queen, BBoard black_king) :

    white_pawn_(white_pawn), white_knight_(white_knight), white_bishop_(white_bishop), white_rook_(white_rook), white_queen_(white_queen), white_king_(white_king),
    black_pawn_(black_pawn), black_knight_(black_knight), black_bishop_(black_bishop), black_rook_(black_rook), black_queen_(black_queen), black_king_(black_king),

    white_(white_pawn_ | white_knight_ | white_bishop_ | white_rook_ | white_queen_ | white_king_),
    black_(black_pawn_ | black_knight_ | black_bishop_ | black_rook_ | black_queen_ | black_king_),
    occ_(white_ | black_)
    {

    }

    Board(
    BBoard white_pawn, BBoard white_knight, BBoard white_bishop, BBoard white_rook, BBoard white_queen, BBoard white_king,
    BBoard black_pawn, BBoard black_knight, BBoard black_bishop, BBoard black_rook, BBoard black_queen, BBoard black_king, 
    BoardState state) :

    white_pawn_(white_pawn), white_knight_(white_knight), white_bishop_(white_bishop), white_rook_(white_rook), white_queen_(white_queen), white_king_(white_king),
    black_pawn_(black_pawn), black_knight_(black_knight), black_bishop_(black_bishop), black_rook_(black_rook), black_queen_(black_queen), black_king_(black_king),

    white_(white_pawn_ | white_knight_ | white_bishop_ | white_rook_ | white_queen_ | white_king_),
    black_(black_pawn_ | black_knight_ | black_bishop_ | black_rook_ | black_queen_ | black_king_),
    occ_(white_ | black_),
    state_(state)
    {

    }

    Board(std::string_view fen_string) : 
    Board(FEN::FenToBmp(fen_string, 'P'), FEN::FenToBmp(fen_string, 'N'), FEN::FenToBmp(fen_string, 'B'), FEN::FenToBmp(fen_string, 'R'), FEN::FenToBmp(fen_string, 'Q'), FEN::FenToBmp(fen_string, 'K') , 
        FEN::FenToBmp(fen_string, 'p'), FEN::FenToBmp(fen_string, 'n'), FEN::FenToBmp(fen_string, 'b'), FEN::FenToBmp(fen_string, 'r'), FEN::FenToBmp(fen_string, 'q'), FEN::FenToBmp(fen_string, 'k'),
        BoardState(fen_string))       
    {

    }
};

namespace BoardInfo {

FORCEINL BBoard getEnemyOrVoid (const Board& board) {
    if (board.state_.white_acts_) return ~board.white_;
    else return ~board.black_;  
}

static char mapBitToPieceType(uint64_t bit, const Board& brd)
{
    if (bit & brd.white_pawn_)    return 'P';
    if (bit & brd.white_knight_)  return 'N';
    if (bit & brd.white_bishop_)  return 'B';
    if (bit & brd.white_rook_)    return 'R';
    if (bit & brd.white_queen_)   return 'Q';
    if (bit & brd.white_king_)    return 'K';
    if (bit & brd.black_pawn_)    return 'p';
    if (bit & brd.black_knight_)  return 'n';
    if (bit & brd.black_bishop_)  return 'b';
    if (bit & brd.black_rook_)    return 'r';
    if (bit & brd.black_queen_)   return 'q';
    if (bit & brd.black_king_)    return 'k';

    return '.';
}

static std::string _map(uint64_t val1, uint64_t val2, const Board& val3, const Board& val4)
{
    static std::string str(64 * 4 + 4 * 8, 'o'); //2 boards with space between

    for (uint64_t i = 0, c = 0; i < 64; i++)
    {
        uint64_t bitmask = (1ull << 63) >> i;

        if ((bitmask & val1) != 0) str[c] = 'X';
        else str[c] = '.';

        if ((bitmask & val2) != 0) str[c + 9] = 'X';
        else str[c + 9] = '.';

        str[c + 18] = mapBitToPieceType(bitmask, val3);
        str[c + 27] = mapBitToPieceType(bitmask, val4);

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



};



}; //eof namespace