#pragma once

#pragma once

#include "../blowfish/defs.h"
#include "../blowfish/chessboard.h"

FORCEINL bool HasMatingMaterial(const Board& board) {
    if( (board.white_rook_  | board.black_rook_) ||
        (board.white_pawn_  | board.black_pawn_) ||
        (board.white_queen_ | board.black_queen_) ) return true;

    if(BitCount(board.white_ | board.black_) < 4 ) return false;

    if(board.white_knight_ | board.black_knight_) return true;

/*     BBoard lq_bishop = 0x55AA55AA55AA55AAULL;
    BBoard dq_bishop = 0xAA55AA55AA55AA55ULL;

    BBoard lq_at = (board.white_bishop_ | board.black_bishop_) & lq_bishop;
    BBoard dq_at = (board.white_bishop_ | board.black_bishop_) & dq_bishop; */
    return true;

    //return lq_at || dq_at;
}



