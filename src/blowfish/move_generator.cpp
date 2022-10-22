#include "move_generator.h"

#include <iostream>

#include "chessboard_extractor.h"
#include "static_move_tables.h"

void IMoveGenerator::GetPseudoLegalMoves(const Board& board) {  
    movecounter_ = 0;
   

    SetEnemyOrVoid(board);
    GetPawnMoves(board);
    GetKnightMoves(board);
/*     GetBishopMoves(board);
    GetRookMoves(board);
    GetKingMoves(board); */

}

void WhiteMoveGenerator::SetEnemyOrVoid(const Board& board) {
    enemy_or_void_ = GetBlackOrVoid(board);


}

void WhiteMoveGenerator::GetPawnMoves(const Board & board) {
    
}

void WhiteMoveGenerator::GetKnightMoves(const Board & board) {
    BBoard knights = board.white_knight_; 

    LoopBits(knights) {
        Square x = LSquare(knights);

        BBoard moves = Lookup::Knight(x) & enemy_or_void_; 

        while(moves) {            
            Square to = PopBit(moves); 
            std::cout << "N || " <<  notations[x] << " || " << notations[LeastBit(to)] << std::endl; //replacing this with a callback in the future.
            ++movecounter_;
        }
    } 

}

void WhiteMoveGenerator::GetBishopMoves(const Board & board) {
    BBoard bishops = board.white_bishop_; 

    LoopBits(bishops) {
        Square x = LSquare(bishops);
        BBoard moves = Lookup::Bishop(x, board.occ_) & enemy_or_void_;   

        std::cout << PrintBoardsAndMask(enemy_or_void_,enemy_or_void_,board,board) << std::endl;       

        while(moves) {            
            Square to = PopBit(moves); 
            std::cout << "B || " <<  x << " || " << notations[LSquare(to)] << std::endl; //replacing this with a callback in the future.
            ++movecounter_;
        }
    } 
}

void WhiteMoveGenerator::GetRookMoves(const Board & board) {

}

void WhiteMoveGenerator::GetKingMoves(const Board & board) {

}