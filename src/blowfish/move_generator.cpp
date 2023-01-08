#include "move_generator.h"

#include <iostream>
#include <assert.h>

//consider removing the movetype evaluation for each insertion, maybe just faster too check on castle legality if we have a rook in corret place?

void IMoveGenerator::GetPseudoLegalMoves(const Board& board, SearchType search_type, int & depth, int & rollout_nr_gen) {  
    movecounter_ = 0;   
    depth_ = depth;
    search_type_ = search_type;   
    enemy_or_void_ = ~board.white_;


    
/*     metadata_reg_->RefreshMetaData(board);  

    checkmask_          = metadata_reg_->checkmask;
    rook_pins_          = metadata_reg_->rook_pins;
    bishop_pins_        = metadata_reg_->bishop_pins;
    moveable_squares_   = enemy_or_void_ & checkmask_;
    kingban_            = metadata_reg_->kingban;
    enp_target_         = metadata_reg_->enp_target;
    nocheck_            = (checkmask_ == 0xffffffffffffffffull); */



    //if checkmask something then disregard other legal moves right away
/*     GetKingMoves(board);

    //else proceed these/
    GetPawnMoves(board);
    GetKnightMoves(board);
    GetBishopMoves(board);    
    GetRookMoves(board); 
    GetQueenMoves(board);

    if(nocheck_) GetCastlingMoves(board);   */  
}


void WhiteMoveGenerator::RefreshMetaDataInternal(const Board& board) {
    kingmoves_ =  Lookup::King(LSquare(board.white_king_)); 
    ekingmoves_ = Lookup::King(LSquare(board.black_king_));

    //Pawn checks
    {
        const BBoard pl = Black_Pawn_AttackLeft(board.black_pawn_ & Pawns_NotLeft());
        const BBoard pr = Black_Pawn_AttackRight(board.black_pawn_ & Pawns_NotRight());

        kingban_ |= (pl | pr);

        if (pl & board.white_king_) check_status_ = Black_Pawn_AttackRight(board.white_king_);
        else if (pr & board.white_king_) check_status_ = Black_Pawn_AttackLeft(board.white_king_);
        else check_status_ = 0xffffffffffffffffull;
    }

    //Knight checks
    {
        BBoard knightcheck = Lookup::Knight(LSquare(board.white_king_)) & board.black_knight_;
        if (knightcheck) check_status_ = knightcheck;
    }

    checkmask_ = check_status_;


    const BBoard king   = board.white_king_;
    const BBoard eking  = board.black_king_;
    const BBoard kingsq = LSquare(king);

    //evaluate pinned pieces and checks from sliders
    {
        rook_pins_ = 0; 
        bishop_pins_ = 0;
        
        if (Chess_Lookup::RookMask[kingsq] & (board.black_rook_ | board.black_queen_))
        {
            BBoard atkHV = Lookup::Rook(kingsq, board.occ_) & (board.black_rook_ | board.black_queen_);
            LoopBits(atkHV) {
                Square sq = LSquare(atkHV);
                CheckBySlider(kingsq, sq);
            }

            BBoard pinnersHV = Lookup::Rook_Xray(kingsq, board.occ_) & (board.black_rook_ | board.black_queen_);
            LoopBits(pinnersHV)
            {                
                RegisterPinHorisontalVertical(kingsq, LSquare(pinnersHV), board);
            }
        }
        if (Chess_Lookup::BishopMask[kingsq] & (board.black_bishop_ | board.black_queen_)) {
            BBoard atkD12 = Lookup::Bishop(kingsq, board.occ_) & (board.black_bishop_ | board.black_queen_);
            LoopBits(atkD12) {
                Square sq = LSquare(atkD12);
                CheckBySlider(kingsq, sq);
            }

            BBoard pinnersD12 = Lookup::Bishop_Xray(kingsq, board.occ_) & (board.black_bishop_ | board.black_queen_);
            LoopBits(pinnersD12)
            {               
                RegisterPinDiagonal(kingsq, LSquare(pinnersD12), board);
            }
        } 
      
        if(board.enp_ != -1) {     
            enp_target_ = 1ULL << board.enp_;

            const BBoard pawns = board.white_pawn_;
            const BBoard enemy_rook_queens = board.black_rook_ | board.black_queen_;
            const BBoard enp64 = 1ULL << board.enp_;

            //Special Horizontal1 https://lichess.org/editor?fen=8%2F8%2F8%2F1K1pP1q1%2F8%2F8%2F8%2F8+w+-+-+0+1
            //Special Horizontal2 https://lichess.org/editor?fen=8%2F8%2F8%2F1K1pP1q1%2F8%2F8%2F8%2F8+w+-+-+0+1

            //King is on EP rank and enemy HV walker is on same rank

            //Remove enemy EP and own EP Candidate from OCC and check if Horizontal path to enemy Slider is open
            //Quick check: We have king - Enemy Slider - Own Pawn - and enemy EP on the same rank!
            if ((WhiteEPRank() & king) && (WhiteEPRank() & enemy_rook_queens) && (WhiteEPRank() & pawns))
            {
                BBoard EPLpawn = pawns & Pawns_NotLeft()  & (White_Pawn_InvertLeft(enp64)); //Pawn that can EPTake to the left - overflow will not matter because 'Notleft'
                BBoard EPRpawn = pawns & Pawns_NotRight() & (White_Pawn_InvertRight(enp64));  //Pawn that can EPTake to the right - overflow will not matter because 'NotRight'

                //invalidates EP from both angles
                if (EPLpawn) {
                    BBoard AfterEPocc = board.occ_ & ~((enp_target_ >> 8) | EPLpawn);
                    if ((Lookup::Rook(kingsq, AfterEPocc) & WhiteEPRank()) & enemy_rook_queens) 
                        enp_target_ = 0x0;
                }
                if (EPRpawn) {
                    BBoard AfterEPocc = board.occ_ & ~((enp_target_ >> 8) | EPRpawn);                    
                    if ((Lookup::Rook(kingsq, AfterEPocc) & WhiteEPRank()) & enemy_rook_queens) 
                        enp_target_ = 0x0;
                }
            }           
        }
    } 
       
    {        
        BBoard knights = board.black_knight_;
        LoopBits(knights)
        {
            kingban_ |= Lookup::Knight(LSquare(knights));
        }
    }

    // Calculate Enemy Bishop
    {
        BBoard bishops = board.black_bishop_;
        LoopBits(bishops)
        {
            const Square sq = LSquare(bishops);                
            BBoard atk = Lookup::Bishop(sq, board.occ_);
            kingban_ |= atk;
        }
    }

    // Calculate Enemy Rook
    {
        BBoard rooks = (board.black_rook_ | board.black_queen_);
        LoopBits(rooks)
        {
            const Square sq = LSquare(rooks);
            BBoard atk = Lookup::Rook(sq, board.occ_);
            kingban_ |= atk;
        }
    }
}

WhiteMoveGenerator::WhiteMoveGenerator()   
{ 
    metadata_reg_ = std::make_unique<WhiteMetaDataRegister> ();
    legal_moves_.reserve(max_allocation_size_);
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

            #ifdef _DEBUG 
                UpdatePawnPromotion(board, PieceType::KNIGHT, pos, to);
                UpdatePawnPromotion(board, PieceType::BISHOP, pos, to);
                UpdatePawnPromotion(board, PieceType::ROOK, pos, to);
                UpdatePawnPromotion(board, PieceType::QUEEN, pos, to);            
            #endif      
        }
        while (Promote_Right)   { 
            const Bit pos = PopBit(Promote_Right);    const Square to = White_Pawn_AttackRight(pos);

            #ifdef _DEBUG
                UpdatePawnPromotion(board, PieceType::KNIGHT, pos, to);
                UpdatePawnPromotion(board, PieceType::BISHOP, pos, to);
                UpdatePawnPromotion(board, PieceType::ROOK, pos, to);
                UpdatePawnPromotion(board, PieceType::QUEEN, pos, to); 
            #endif
        }
        while (Promote_Move)    { 
            const Bit pos = PopBit(Promote_Move);     const Square to = White_Pawn_Forward(pos);

            #ifdef _DEBUG 
                UpdatePawnPromotion(board, PieceType::KNIGHT, pos, to);
                UpdatePawnPromotion(board, PieceType::BISHOP, pos, to);
                UpdatePawnPromotion(board, PieceType::ROOK, pos, to);
                UpdatePawnPromotion(board, PieceType::QUEEN, pos, to); 
            #endif 
        }
        while (NoPromote_Left)  { 
            const Bit pos = PopBit(NoPromote_Left);   const Square to = White_Pawn_AttackLeft(pos);

            #ifdef _DEBUG 
                UpdatePawnCapture(board, pos, to);
            #endif
            
        }
        while (NoPromote_Right) { 
            const Bit pos = PopBit(NoPromote_Right);  const Square to = White_Pawn_AttackRight(pos);

            #ifdef _DEBUG 
                UpdatePawnCapture(board, pos, to);
            #endif
        }
        while (NoPromote_Move)  { 
            const Bit pos = PopBit(NoPromote_Move);   const Square to = White_Pawn_Forward(pos);

            #ifdef _DEBUG 
                UpdatePawnMove(board, pos, to);
            #endif            
            
        }
        while (pawn_forward_2)  { 
            const Bit pos = PopBit(pawn_forward_2);   const Square to = White_Pawn_Forward2(pos);

            #ifdef _DEBUG 
                UpdatePawnPush(board, pos, to);
            #endif           
        }
    }
    else { 
        while (pawn_capture_left)  { 
            const Bit pos = PopBit(pawn_capture_left);  const Square to = White_Pawn_AttackLeft(pos);
            #ifdef _DEBUG
                UpdatePawnCapture(board, pos, to);
            #endif
        }
        while (pawn_capture_right) { 
            const Bit pos = PopBit(pawn_capture_right); const Square to = White_Pawn_AttackRight(pos);

            #ifdef _DEBUG
                UpdatePawnCapture(board, pos, to);
            #endif
        }
        while (pawn_forward_1)     { 
            const Bit pos = PopBit(pawn_forward_1);     const Square to = White_Pawn_Forward(pos);

            #ifdef _DEBUG
                UpdatePawnMove(board, pos, to);
            #endif
        }
        while (pawn_forward_2)     { 
            const Bit pos = PopBit(pawn_forward_2);     const Square to = White_Pawn_Forward2(pos);

            #ifdef _DEBUG
                UpdatePawnPush(board, pos, to);
            #endif
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
            #ifdef _DEBUG
                UpdateKnightMove(board, x, to);
            #endif       
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

                #ifdef _DEBUG
                    UpdateQueenMove(board, x, to);
                #endif
            }
        }
        else {
             while(moves)  {
                Square to = PopBit(moves); 

                #ifdef _DEBUG
                    UpdateBishopMove(board, x, to);
                #endif
            }          
        }     
    } 

    LoopBits(nopin_bishops) {
        Square x = LSquare(bishops);

        BBoard moves = Lookup::Bishop(x, board.occ_) & moveable_squares_; 

        while(moves) {            
            Square to = PopBit(moves); 
            #ifdef _DEBUG
                UpdateBishopMove(board, x, to);
            #endif
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
            #ifdef _DEBUG 
                UpdateRookMove(board, x, to);
            #endif            
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
            #ifdef _DEBUG
                UpdateQueenMove(board, x, to);
            #endif
        } 
    } 
}

void WhiteMoveGenerator::GetKingMoves(const Board & board) {  
    Square x = LSquare(board.white_king_);
    BBoard moves = Lookup::King(x) &~ (kingban_ | board.white_);

    /* BoardConsoleGUI::PrintBoard(moveable_squares_); */
    while(moves) {            
        Square to = PopBit(moves);    
        #ifdef _DEBUG
            UpdateKingMove(board, x, to);
        #endif
    }

}

void WhiteMoveGenerator::GetCastlingMoves(const Board& board) {    
    if(board.white_oo_ && !((board.occ_ | kingban_) & wRCastleInterferenceSquares)) {
        //we could check if rook and king is in correct position, but we should handle this in state transition, Or maybe it should be handled here... 
        //decided, add check that rook is in place.
        #ifdef _DEBUG
            UpdateCastle00(board);
        #endif
    }

    if(board.white_ooo_ && !((board.occ_ | kingban_) & wLCastleInterferenceSquares)) {
        #ifdef _DEBUG
            UpdateCastle000(board);
        #endif
    }
}


