#include "move_generator.h"

#include <iostream>
#include <assert.h>





void IMoveGenerator::GetPseudoLegalMoves(const Board& board, SearchType search_type, int & depth, int & rollout_nr_gen) {  
    movecounter_ = 0;   
    depth_ = depth;

    search_type_ = search_type;

    SetEnemyOrVoid(board);
    
    metadata_reg_->RefreshMetaData(board);  

    checkmask_          = metadata_reg_->checkmask;
    rook_pins_          = metadata_reg_->rook_pins;
    bishop_pins_        = metadata_reg_->bishop_pins;
    moveable_squares_   = enemy_or_void_ & checkmask_;
    kingban_            = metadata_reg_->kingban;
    enp_target_         = metadata_reg_->enp_target;
    nocheck_            = (checkmask_ == 0xffffffffffffffffull);

    //if checkmask something then disregard other legal moves right away
    GetKingMoves(board);

    //else proceed these/
    GetPawnMoves(board);
    GetKnightMoves(board);
    GetBishopMoves(board);    
    GetRookMoves(board); 
    GetQueenMoves(board);

    if(nocheck_)  GetCastlingMoves(board);    
}

WhiteMoveGenerator::WhiteMoveGenerator()   
{ 
    metadata_reg_ = std::make_unique<WhiteMetaDataRegister> ();
}

void WhiteMoveGenerator::SetEnemyOrVoid(const Board& board) {
    enemy_or_void_ = GetBlackOrVoid(board);
}


//section logic merely copied from gargantua
void WhiteMoveGenerator::GetPawnMoves(const Board & board) {
    //start by defining which pawns can move diagonally and which can move forward
    const BBoard pawns_lr = board.white_pawn_ &~ rook_pins_;
    const BBoard pawns_hv = board.white_pawn_ &~ bishop_pins_;

    //define which pawns can capture left/right, move forward and move forward 2 steps
    //guess this can be done in two ways, but the big brain style is to start from the enemy occupant squares and invert back to the capture position.
    //capture left / right
    BBoard pawn_capture_left  = pawns_lr & White_Pawn_InvertLeft(board.black_ & Pawns_NotRight() & checkmask_);
    BBoard pawn_capture_right  = pawns_lr & White_Pawn_InvertRight(board.black_ & Pawns_NotLeft() & checkmask_);    

    //forward
    BBoard pawn_forward_1 = pawns_hv & White_Pawn_Backward(~board.occ_); // no checkmask needed here? why? it comes later

    //push, pawn is on first rank, the slot two ranks forward is free, forward 1 is eligible
    uint64_t pawn_forward_2 = pawn_forward_1 & White_Pawns_FirstRank() & White_Pawn_Backward2(~board.occ_ & checkmask_);

    pawn_forward_1 &= White_Pawn_Backward(checkmask_); //there is an optimization reason to move this after determining pawn forward 2, 
    //with the following comment https://lichess.org/editor?fen=rnbpkbnr%2Fppp3pp%2F4pp2%2Fq7%2F8%2F3P4%2FPPP1PPPP%2FRNBQKBNR+w+KQkq+-+0+1
    //which i honestly dont fully understand

    White_Pawns_PruneMove(pawn_forward_1, rook_pins_);
    White_Pawns_PruneMove2(pawn_forward_2, rook_pins_);
    White_Pawns_PruneLeft(pawn_capture_left, bishop_pins_);
    White_Pawns_PruneRight(pawn_capture_right, bishop_pins_);

    //handle ENP
    if (enp_target_) {  
        //The eppawn must be an enemy since its only ever valid for a single move
        BBoard EPLpawn = pawns_lr & Pawns_NotLeft()  & (White_Pawn_InvertLeft(enp_target_  & checkmask_)); //Pawn that can EPTake to the left - overflow will not matter because 'Notleft'
        BBoard EPRpawn = pawns_lr & Pawns_NotRight() & (White_Pawn_InvertRight(enp_target_ & checkmask_));  //Pawn that can EPTake to the right - overflow will not matter because 'NotRight'


        //Special check for pinned EP Take - which is a very special move since even XRay does not see through the 2 pawns on a single rank
        // White will push you cannot EP take: https://lichess.org/editor?fen=8%2F7B%2F8%2F8%2F4p3%2F3k4%2F5P2%2F8+w+-+-+0+1
        // White will push you cannot EP take: https://lichess.org/editor?fen=8%2F8%2F8%2F8%2F1k2p2R%2F8%2F5P2%2F8+w+-+-+0+1
        
        if (EPLpawn | EPRpawn) //Todo: bench if slower or faster
        {
            White_Pawn_PruneLeftEP(EPLpawn, bishop_pins_);
            White_Pawn_PruneRightEP(EPRpawn, bishop_pins_);

            if(EPLpawn) { const Bit pos = PopBit(EPLpawn);     const Square to = White_Pawn_AttackLeft(pos); 
            std::cout << "P(EP LEFT) || " <<  notations[LSquare(pos)] << " || " << notations[LeastBit(to)] << std::endl;} // add legal EP move
            if(EPRpawn) { const Bit pos = PopBit(EPRpawn);    const Square to = White_Pawn_AttackRight(pos);
            std::cout << "P(EP RIGHT) || " <<  notations[LSquare(pos)] << " || " << notations[LeastBit(to)] << std::endl; } // add legal EP move
        }
    }

    //Handle PROMOTION and make moves from all legal pawn moves pruned

    if ((pawn_capture_left | pawn_capture_right | pawn_forward_1) & White_Pawns_LastRank()) {
        uint64_t Promote_Left =  pawn_capture_left & White_Pawns_LastRank();  
        uint64_t Promote_Right = pawn_capture_right & White_Pawns_LastRank(); 
        uint64_t Promote_Move =  pawn_forward_1 & White_Pawns_LastRank(); 

        uint64_t NoPromote_Left =  pawn_capture_left & ~White_Pawns_LastRank(); 
        uint64_t NoPromote_Right = pawn_capture_right & ~White_Pawns_LastRank();
        uint64_t NoPromote_Move =  pawn_forward_1 & ~White_Pawns_LastRank();        

        //we treat promo transitions different
        while (Promote_Left)    { 
            const Bit pos = PopBit(Promote_Left);     const Square to = White_Pawn_AttackLeft(pos); 
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPromote, PieceType::KNIGHT, pos, to), search_type_, depth_ + 1);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPromote, PieceType::BISHOP, pos, to), search_type_, depth_ + 1);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPromote, PieceType::ROOK, pos, to), search_type_, depth_ + 1);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPromote, PieceType::QUEEN, pos, to), search_type_, depth_ + 1);
        }
        while (Promote_Right)   { 
            const Bit pos = PopBit(Promote_Right);    const Square to = White_Pawn_AttackRight(pos);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPromote, PieceType::KNIGHT, pos, to), search_type_, depth_ + 1);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPromote, PieceType::BISHOP, pos, to), search_type_, depth_ + 1);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPromote, PieceType::ROOK, pos, to), search_type_, depth_ + 1);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPromote, PieceType::QUEEN, pos, to), search_type_, depth_ + 1);
        }
        while (Promote_Move)    { 
            const Bit pos = PopBit(Promote_Move);     const Square to = White_Pawn_Forward(pos);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPromote, PieceType::KNIGHT, pos, to), search_type_, depth_ + 1);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPromote, PieceType::BISHOP, pos, to), search_type_, depth_ + 1);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPromote, PieceType::ROOK, pos, to), search_type_, depth_ + 1);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPromote, PieceType::QUEEN, pos, to), search_type_, depth_ + 1);
        }
        while (NoPromote_Left)  { 
            const Bit pos = PopBit(NoPromote_Left);   const Square to = White_Pawn_AttackLeft(pos);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnCapture, PieceType::PAWN, pos, to), search_type_, depth_ + 1);
        }
        while (NoPromote_Right) { 
            const Bit pos = PopBit(NoPromote_Right);  const Square to = White_Pawn_AttackRight(pos);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnCapture, PieceType::PAWN, pos, to), search_type_, depth_ + 1);
        }
        while (NoPromote_Move)  { 
            const Bit pos = PopBit(NoPromote_Move);   const Square to = White_Pawn_Forward(pos);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnMove, PieceType::PAWN, pos, to), search_type_, depth_ + 1);
            }
        while (pawn_forward_2)  { 
            const Bit pos = PopBit(pawn_forward_2);   const Square to = White_Pawn_Forward2(pos);
            callback_.Trigger(::UpdateMove(board, MoveTypes::PawnPush, PieceType::PAWN, pos, to), search_type_, depth_ + 1);
            }
    }
    else { //M_Callback(UpdateBoardWhitePawnCaptureLeft(board, pos, to), SearchType::PERFT, depth_ + 1);
        while (pawn_capture_left)  { 
            const Bit pos = PopBit(pawn_capture_left);  const Square to = White_Pawn_AttackLeft(pos);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnCapture, PieceType::PAWN, pos, to), search_type_, depth_ + 1);
        }
        while (pawn_capture_right) { 
            const Bit pos = PopBit(pawn_capture_right); const Square to = White_Pawn_AttackRight(pos);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnCapture, PieceType::PAWN, pos, to), search_type_, depth_ + 1);
        }
        while (pawn_forward_1)     { 
            const Bit pos = PopBit(pawn_forward_1);     const Square to = White_Pawn_Forward(pos);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnMove, PieceType::PAWN, pos, to), search_type_, depth_ + 1);
        }
        while (pawn_forward_2)     { 
            const Bit pos = PopBit(pawn_forward_2);     const Square to = White_Pawn_Forward2(pos);
            callback_.Trigger(UpdateMove(board, MoveTypes::PawnPush, PieceType::PAWN, pos, to), search_type_, depth_ + 1);
        }
    }    
}

void WhiteMoveGenerator::GetKnightMoves(const Board & board) {
    BBoard knights = board.white_knight_ & (~ (rook_pins_ | bishop_pins_)); //a pinned knight can never move regardless 

    LoopBits(knights) {
        Square x = LSquare(knights);    

        BBoard moves = Lookup::Knight(x) & moveable_squares_; 

        while(moves) {            
            Square to = PopBit(moves); 
             std::cout << "N || " <<  notations[x] << " || " << notations[LeastBit(to)] << std::endl; //replacing this with a callback in the future.
            ++movecounter_; 
        }
    } 
}

void WhiteMoveGenerator::GetBishopMoves(const Board & board) {

    BBoard queens = board.white_queen_; 

    BBoard bishops = board.white_bishop_ &~ rook_pins_; //including non pinned bishops (HV) 
    
    BBoard pinned_bishops = (bishops | queens) & bishop_pins_; //including pinned queens diagonally here.
    BBoard nopin_bishops = bishops & ~bishop_pins_;

    LoopBits(pinned_bishops) {
        Square x = LSquare(pinned_bishops);

        BBoard moves = Lookup::Bishop(x, board.occ_) & moveable_squares_ & bishop_pins_; 

        if(1ULL << x & queens) {
            
            while(moves)  {
                Square to = PopBit(moves); 
                std::cout << "Q || " <<  notations[x] << " || " << notations[LeastBit(to)] << std::endl; //replacing this with a callback in the future.
                ++movecounter_;
            }
        }
        else {
             while(moves)  {
                Square to = PopBit(moves); 
                std::cout << "B || " <<  notations[x] << " || " << notations[LeastBit(to)] << std::endl; //replacing this with a callback in the future.
                ++movecounter_; 
            }          
        }     
    } 

    LoopBits(nopin_bishops) {
        Square x = LSquare(bishops);

        BBoard moves = Lookup::Bishop(x, board.occ_) & moveable_squares_; 

        while(moves) {            
            Square to = PopBit(moves); 
            std::cout << "B || " <<  notations[x] << " || " << notations[LeastBit(to)] << std::endl; //replacing this with a callback in the future.
            ++movecounter_;
        }
    } 
}

void WhiteMoveGenerator::GetRookMoves(const Board & board) {
    BBoard rooks = board.white_rook_; 

    LoopBits(rooks) {
        Square x = LSquare(rooks);

        BBoard moves = Lookup::Rook(x, board.occ_) & moveable_squares_; 

        while(moves) {            
            Square to = PopBit(moves); 
            std::cout << "R || " <<  notations[x] << " || " << notations[LeastBit(to)] << std::endl; //replacing this with a callback in the future.
            ++movecounter_;
        }
    } 
}

void WhiteMoveGenerator::GetQueenMoves(const Board & board) {
    BBoard queens = board.white_queen_; 

    LoopBits(queens) {
        Square x = LSquare(queens);
        BBoard moves = Lookup::Queen(x, board.occ_) & moveable_squares_; 

        while(moves) {            
            Square to = PopBit(moves); 
            std::cout << "Q || " <<  notations[x] << " || " << notations[LeastBit(to)] << std::endl; //replacing this with a callback in the future.
            ++movecounter_;
        } 
    } 
}

void WhiteMoveGenerator::GetKingMoves(const Board & board) {  
    Square x = LSquare(board.white_king_);
    BBoard moves = Lookup::King(x) &~ (kingban_ | board.white_);

    /* BoardConsoleGUI::PrintBoard(moveable_squares_); */
    while(moves) {            
        Square to = PopBit(moves); 
         std::cout << "K || " <<  notations[LSquare(board.white_king_)] << " || " << notations[LeastBit(to)] << std::endl; //replacing this with a callback in the future.
        ++movecounter_;  
    }

}

void WhiteMoveGenerator::GetCastlingMoves(const Board& board) {    
    if(board.state_.white_oo_ && !((board.occ_ | kingban_) & wRCastleInterferenceSquares)) {
        //we could check if rook and king is in correct position, but we should handle this in state transition 
        std::cout << "O-O :  || " <<  notations[LSquare(board.white_king_)] << " || " << notations[6] << std::endl;
    }

    if(board.state_.white_ooo_ && !((board.occ_ | kingban_) & wLCastleInterferenceSquares)) {
        std::cout << "O-O-O :  || " <<  notations[LSquare(board.white_king_)] << " || " << notations[2] << std::endl;
    }
}


