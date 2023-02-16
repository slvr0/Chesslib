#include "bmovegen.h"

#include "move_generator.h"

MGSearchContextualObject BlackMoveGenerator::RefreshMetaDataInternal(const Board& board) {
    MGSearchContextualObject context;
    context.kingmoves_ =  Lookup::King(LSquare(board.black_king_)); 
    context.ekingmoves_ = Lookup::King(LSquare(board.white_king_));

    //Pawn checks
    {
        const BBoard pl = White_Pawn_AttackLeft(board.white_pawn_ & Pawns_NotRight());
        const BBoard pr = White_Pawn_AttackRight(board.white_pawn_ & Pawns_NotLeft());
        
        context.kingban_ |= (pl | pr);

        if (pl & board.black_king_) context.check_status_ = Black_Pawn_AttackRight(board.black_king_);
        else if (pr & board.black_king_) context.check_status_ = Black_Pawn_AttackLeft(board.black_king_);
        else context.check_status_ = 0xffffffffffffffffull;
    }

    //Knight checks
    {
        BBoard knightcheck = Lookup::Knight(LSquare(board.black_king_)) & board.white_knight_;
        if (knightcheck) context.check_status_ = knightcheck;
    }

    context.checkmask_ = context.check_status_;

    const BBoard king   = board.black_king_;
    const BBoard eking  = board.white_king_;
    const BBoard kingsq = LSquare(king);

    //evaluate pinned pieces and checks from sliders
    {
        context.rook_pins_ = 0; 
        context.bishop_pins_ = 0;
        
        if (Chess_Lookup::RookMask[kingsq] & (board.white_rook_ | board.white_queen_))
        {
            BBoard atkHV = Lookup::Rook(kingsq, board.occ_) & (board.white_rook_ | board.white_queen_);
            LoopBits(atkHV) {
                Square sq = LSquare(atkHV);
                CheckBySlider(kingsq, sq, context);
            }

            BBoard pinnersHV = Lookup::Rook_Xray(kingsq, board.occ_) & (board.white_rook_ | board.white_queen_);
            LoopBits(pinnersHV)
            {                
                RegisterPinHorisontalVertical(kingsq, LSquare(pinnersHV), board, context);
            }
        }
        if (Chess_Lookup::BishopMask[kingsq] & (board.white_bishop_ | board.white_queen_)) {
            BBoard atkD12 = Lookup::Bishop(kingsq, board.occ_) & (board.white_bishop_ | board.white_queen_);
            LoopBits(atkD12) {
                Square sq = LSquare(atkD12);
                CheckBySlider(kingsq, sq, context);
            }

            BBoard pinnersD12 = Lookup::Bishop_Xray(kingsq, board.occ_) & (board.white_bishop_ | board.white_queen_);
            LoopBits(pinnersD12)
            {               
                RegisterPinDiagonal(kingsq, LSquare(pinnersD12), board, context);
            }
        } 
      
        if(board.enp_ != -1) {     
            context.enp_target_ = 1ULL << board.enp_;

            const BBoard pawns = board.black_pawn_;
            const BBoard enemy_rook_queens = board.white_rook_ | board.white_queen_;
         

            //Special Horizontal1 https://lichess.org/editor?fen=8%2F8%2F8%2F1K1pP1q1%2F8%2F8%2F8%2F8+w+-+-+0+1
            //Special Horizontal2 https://lichess.org/editor?fen=8%2F8%2F8%2F1K1pP1q1%2F8%2F8%2F8%2F8+w+-+-+0+1

            //King is on EP rank and enemy HV walker is on same rank

            //Remove enemy EP and own EP Candidate from OCC and check if Horizontal path to enemy Slider is open
            //Quick check: We have king - Enemy Slider - Own Pawn - and enemy EP on the same rank!
            if ((BlackEPRank() & king) && (BlackEPRank() & enemy_rook_queens) && (BlackEPRank() & pawns))
            {
                BBoard EPLpawn = pawns & Pawns_NotLeft()  & (Black_Pawn_InvertLeft(context.enp_target_)); //Pawn that can EPTake to the left - overflow will not matter because 'Notleft'
                BBoard EPRpawn = pawns & Pawns_NotRight() & (Black_Pawn_InvertRight(context.enp_target_));  //Pawn that can EPTake to the right - overflow will not matter because 'NotRight'

                //invalidates EP from both angles
                if (EPLpawn) {
                    BBoard AfterEPocc = board.occ_ & ~((context.enp_target_ >> 8) | EPLpawn);
                    if ((Lookup::Rook(kingsq, AfterEPocc) & BlackEPRank()) & enemy_rook_queens) 
                        context.enp_target_ = 0x0;
                }
                if (EPRpawn) {
                    BBoard AfterEPocc = board.occ_ & ~((context.enp_target_ >> 8) | EPRpawn);                    
                    if ((Lookup::Rook(kingsq, AfterEPocc) & BlackEPRank()) & enemy_rook_queens) 
                        context.enp_target_ = 0x0;
                }
            }           
        }
    } 
       
    {        
        BBoard knights = board.white_knight_;
        LoopBits(knights)
        {
            context.kingban_ |= Lookup::Knight(LSquare(knights));
        }
    }

    // Calculate Enemy Bishop
    {
        BBoard bishops = board.white_bishop_ | board.white_queen_;
        LoopBits(bishops)
        {
            const Square sq = LSquare(bishops);                
            BBoard atk = Lookup::Bishop(sq, board.occ_);
            context.kingban_ |= atk;
        }
    }

    // Calculate Enemy Rook
    {
        BBoard rooks = (board.white_rook_ | board.white_queen_);
        LoopBits(rooks)
        {
            const Square sq = LSquare(rooks);
            BBoard atk = Lookup::Rook(sq, board.occ_);
            context.kingban_ |= atk;
        }
    }

    context.kingban_ |= context.ekingmoves_;

    return context;
}

BlackMoveGenerator::BlackMoveGenerator(MoveGeneratorHeader* parent) : 
    parent_(parent)   
{ 
   
}


bool BlackMoveGenerator::ParseLegalMoves(const Board& board, const int& depth, const bool& debug) {

    Timer t0;

    MGSearchContextualObject context_object = RefreshMetaDataInternal(board);
    metadata_searchtime += t0.elapsed();

    context_object.depth_ = depth;      
    context_object.enemy_or_void_ = ~board.black_;
    context_object.nocheck_ = (bool)(context_object.checkmask_ == 0xffffffffffffffffull); 

    
    context_object.moveable_squares_   = context_object.enemy_or_void_ & context_object.checkmask_;

    context_object.debug_ = debug;    

    GetKingMoves(board, context_object);

    if(context_object.checkmask_) {    
        GetPawnMoves(board,     context_object);
        GetKnightMoves(board,   context_object);
        GetBishopMoves(board,   context_object);    
        GetRookMoves(board,     context_object); 
        GetQueenMoves(board,    context_object);

        if(context_object.nocheck_) GetCastlingMoves(board, context_object);     
    }
    return context_object.nocheck_;
}

void BlackMoveGenerator::GetPawnMoves(const Board & board, MGSearchContextualObject & context) {  
    const BBoard pawns_lr = board.black_pawn_ &~ context.rook_pins_;
    const BBoard pawns_hv = board.black_pawn_ &~ context.bishop_pins_;

    BBoard pawn_capture_left  = pawns_lr & Black_Pawn_InvertLeft(board.white_ & Pawns_NotLeft() & context.checkmask_);
    BBoard pawn_capture_right  = pawns_lr & Black_Pawn_InvertRight(board.white_ & Pawns_NotRight() & context.checkmask_);  

    //forward
    BBoard pawn_forward_1 = pawns_hv & Black_Pawn_Backward(~board.occ_); // no checkmask needed here? why? it comes later

    //push, pawn is on first rank, the slot two ranks forward is free, forward 1 is eligible
    uint64_t pawn_forward_2 = pawn_forward_1 & Black_Pawns_FirstRank() & Black_Pawn_Backward2(~board.occ_ & context.checkmask_);

    pawn_forward_1 &= Black_Pawn_Backward(context.checkmask_); //there is an optimization reason to move this after determining pawn forward 2, 
    //with the following comment https://lichess.org/editor?fen=rnbpkbnr%2Fppp3pp%2F4pp2%2Fq7%2F8%2F3P4%2FPPP1PPPP%2FRNBQKBNR+w+KQkq+-+0+1
    //which i honestly dont fully understand

    Black_Pawns_PruneMove(pawn_forward_1,       context.rook_pins_);
    Black_Pawns_PruneMove2(pawn_forward_2,      context.rook_pins_);
    Black_Pawns_PruneLeft(pawn_capture_left,    context.bishop_pins_);
    Black_Pawns_PruneRight(pawn_capture_right,  context.bishop_pins_);
   
    if (context.enp_target_) {    
        BBoard EPLpawn = pawns_lr  & (Black_Pawn_InvertLeft(context.enp_target_  & Pawns_NotLeft())     & context.checkmask_ << 1); 
        BBoard EPRpawn = pawns_lr  & (Black_Pawn_InvertRight(context.enp_target_ & Pawns_NotRight())    & context.checkmask_ >> 1);   

        if (EPLpawn | EPRpawn)
        {
            Black_Pawn_PruneLeftEP(EPLpawn,     context.bishop_pins_);
            Black_Pawn_PruneRightEP(EPRpawn,    context.bishop_pins_);

            if(EPLpawn) {
                const Bit pos = PopBit(EPLpawn); 
                const Square to = Black_Pawn_AttackLeft(pos); 
                
                const Board epl_board = UpdatePawnEnpassaint(board, pos, to, context.enp_target_ << 8);

                parent_->OnInsert(epl_board, context.depth_ + 1);

                if(context.debug_) 
                    parent_->OnInsertDebug(board, epl_board, notations[LSquare(pos)] + notations[LSquare(to)]);
                
            }

            if(EPRpawn) {
                const Bit pos = PopBit(EPRpawn); 
                const Square to = Black_Pawn_AttackRight(pos); 
                
                const Board epr_board = UpdatePawnEnpassaint(board, pos, to, context.enp_target_ << 8);

                parent_->OnInsert(epr_board, context.depth_ + 1);

                if(context.debug_) 
                    parent_->OnInsertDebug(board, epr_board, notations[LSquare(pos)] + notations[LSquare(to)]);
                
            }
        }
    }

    //Handle PROMOTION and make moves from all legal pawn moves pruned

    if ((pawn_capture_left | pawn_capture_right | pawn_forward_1) & Black_Pawns_LastRank()) {
        uint64_t Promote_Left =  pawn_capture_left & Black_Pawns_LastRank();  
        uint64_t Promote_Right = pawn_capture_right & Black_Pawns_LastRank(); 
        uint64_t Promote_Move =  pawn_forward_1 & Black_Pawns_LastRank(); 

        uint64_t NoPromote_Left =  pawn_capture_left & ~Black_Pawns_LastRank(); 
        uint64_t NoPromote_Right = pawn_capture_right & ~Black_Pawns_LastRank();
        uint64_t NoPromote_Move =  pawn_forward_1 & ~Black_Pawns_LastRank();        

        //we treat promo transitions different
        while (Promote_Left)    { 
            const Bit pos = PopBit(Promote_Left);     const Square to = Black_Pawn_AttackLeft(pos); 

            const Board nb1 = UpdatePawnPromotion(board, PieceType::KNIGHT, pos, to);
            parent_->OnInsert(nb1, context.depth_ + 1);
            const Board nb2 = UpdatePawnPromotion(board, PieceType::BISHOP, pos, to);
            parent_->OnInsert(nb2, context.depth_ + 1);
            const Board nb3 = UpdatePawnPromotion(board, PieceType::ROOK, pos, to);
            parent_->OnInsert(nb3, context.depth_ + 1);
            const Board nb4 = UpdatePawnPromotion(board, PieceType::QUEEN, pos, to);  
            parent_->OnInsert(nb4, context.depth_ + 1);          
             
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb1, notations[LSquare(pos)] + notations[LSquare(to)] + "n");
                parent_->OnInsertDebug(board, nb2, notations[LSquare(pos)] + notations[LSquare(to)] + "b");
                parent_->OnInsertDebug(board, nb3, notations[LSquare(pos)] + notations[LSquare(to)] + "r");
                parent_->OnInsertDebug(board, nb4, notations[LSquare(pos)] + notations[LSquare(to)] + "q");       
                
        }
        while (Promote_Right)   { 
            const Bit pos = PopBit(Promote_Right);    const Square to = Black_Pawn_AttackRight(pos);
            
            const Board nb1 = UpdatePawnPromotion(board, PieceType::KNIGHT, pos, to);
            parent_->OnInsert(nb1, context.depth_ + 1);
            const Board nb2 = UpdatePawnPromotion(board, PieceType::BISHOP, pos, to);
            parent_->OnInsert(nb2, context.depth_ + 1);
            const Board nb3 = UpdatePawnPromotion(board, PieceType::ROOK, pos, to);
            parent_->OnInsert(nb3, context.depth_ + 1);
            const Board nb4 = UpdatePawnPromotion(board, PieceType::QUEEN, pos, to); 
            parent_->OnInsert(nb4, context.depth_ + 1);
            
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb1, notations[LSquare(pos)] + notations[LSquare(to)] + "n");
                parent_->OnInsertDebug(board, nb2, notations[LSquare(pos)] + notations[LSquare(to)] + "b");
                parent_->OnInsertDebug(board, nb3, notations[LSquare(pos)] + notations[LSquare(to)] + "r");
                parent_->OnInsertDebug(board, nb4, notations[LSquare(pos)] + notations[LSquare(to)] + "q");       
             
        }
        while (Promote_Move)    { 
            const Bit pos = PopBit(Promote_Move);     const Square to = Black_Pawn_Forward(pos);
             
            const Board nb1 = UpdatePawnPromotion(board, PieceType::KNIGHT, pos, to);
            parent_->OnInsert(nb1, context.depth_ + 1);
            const Board nb2 = UpdatePawnPromotion(board, PieceType::BISHOP, pos, to);
            parent_->OnInsert(nb2, context.depth_ + 1);
            const Board nb3 = UpdatePawnPromotion(board, PieceType::ROOK, pos, to);
            parent_->OnInsert(nb3, context.depth_ + 1);
            const Board nb4 = UpdatePawnPromotion(board, PieceType::QUEEN, pos, to); 
            parent_->OnInsert(nb4, context.depth_ + 1);
            
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb1, notations[LSquare(pos)] + notations[LSquare(to)] + "n");
                parent_->OnInsertDebug(board, nb2, notations[LSquare(pos)] + notations[LSquare(to)] + "b");
                parent_->OnInsertDebug(board, nb3, notations[LSquare(pos)] + notations[LSquare(to)] + "r");
                parent_->OnInsertDebug(board, nb4, notations[LSquare(pos)] + notations[LSquare(to)] + "q");        
             
        }
        while (NoPromote_Left)  { 
            const Bit pos = PopBit(NoPromote_Left);   const Square to = Black_Pawn_AttackLeft(pos);

            const Board nb = UpdatePawnCapture(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);
           
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)] + notations[LSquare(to)]);
            
            
        }
        while (NoPromote_Right) { 
            const Bit pos = PopBit(NoPromote_Right);  const Square to = Black_Pawn_AttackRight(pos);
            
            const Board nb = UpdatePawnCapture(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);
            
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)] + notations[LSquare(to)]);
            
        }
        while (NoPromote_Move)  { 
            const Bit pos = PopBit(NoPromote_Move);   const Square to = Black_Pawn_Forward(pos);
           
            const Board nb =UpdatePawnMove(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);
             
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)]+ notations[LSquare(to)]);
                        
            
        }
        while (pawn_forward_2)  { 
            const Bit pos = PopBit(pawn_forward_2);   const Square to = Black_Pawn_Forward2(pos);

            const Board nb =UpdatePawnPush(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);
              
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)] + notations[LSquare(to)]);
                   
        }
    }
    else { 
        while (pawn_capture_left)  { 
            const Bit pos = PopBit(pawn_capture_left);  const Square to = Black_Pawn_AttackLeft(pos);
            
            const Board nb =UpdatePawnCapture(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);
            
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)] + notations[LSquare(to)]);
            
        }
        while (pawn_capture_right) { 
            const Bit pos = PopBit(pawn_capture_right); const Square to = Black_Pawn_AttackRight(pos);

            const Board nb =UpdatePawnCapture(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);
            
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)] + notations[LSquare(to)]);
            
        }
        while (pawn_forward_1)     { 
            const Bit pos = PopBit(pawn_forward_1);     const Square to = Black_Pawn_Forward(pos);

            const Board nb = UpdatePawnMove(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);
        
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)] + notations[LSquare(to)]);
             
        }
        while (pawn_forward_2)     { 
            const Bit pos = PopBit(pawn_forward_2);     const Square to = Black_Pawn_Forward2(pos);

            const Board nb = UpdatePawnPush(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);
            
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)] + notations[LSquare(to)]);
            
        }
    }    
}

void BlackMoveGenerator::GetKnightMoves(const Board & board, MGSearchContextualObject & context) {
    BBoard knights = board.black_knight_ & (~ (context.rook_pins_ | context.bishop_pins_)); //a pinned knight can never move!

    LoopBits(knights) {
        Square x = LSquare(knights);    

        BBoard moves = Lookup::Knight(x) & context.moveable_squares_; 

        while(moves) {            
            Square to = PopBit(moves); 
            
            const Board nb = UpdateKnightMove(board, x, to);
            parent_->OnInsert(nb, context.depth_ + 1);
           
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)]);
                
        }
    } 
}

void BlackMoveGenerator::GetBishopMoves(const Board & board, MGSearchContextualObject & context) {

    BBoard queens = board.black_queen_; 

    BBoard bishops = board.black_bishop_ &~ context.rook_pins_; //including non pinned bishops (HV) 
    
    BBoard pinned_bishops = (bishops | queens) & context.bishop_pins_; //including pinned queens diagonally here.
    BBoard nopin_bishops = bishops & ~context.bishop_pins_; 
    
    LoopBits(pinned_bishops) {
        Square x = LSquare(pinned_bishops);

        BBoard moves = Lookup::Bishop(x, board.occ_) & context.moveable_squares_ & context.bishop_pins_; 

        if((1ULL << x) & board.black_queen_) {
            
            while(moves)  {
                Square to = PopBit(moves); 
            
                const Board nb = UpdateQueenMove(board, x, to);
                parent_->OnInsert(nb, context.depth_ + 1);               

                if(context.debug_) 
                    parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)]);
                  
            }
        }
        else {

             while(moves)  {               

                Square to = PopBit(moves); 
                
                const Board nb = UpdateBishopMove(board, x, to);
                parent_->OnInsert(nb, context.depth_ + 1);                

                if(context.debug_) 
                    parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)]);
                  
            }          
        }     
    } 

    LoopBits(nopin_bishops) {

        Square x = LSquare(nopin_bishops);
    
        BBoard moves = Lookup::Bishop(x, board.occ_) & context.moveable_squares_;  
 

        while(moves) {            
            Square to = PopBit(moves); 
            
            const Board nb = UpdateBishopMove(board, x, to);
            parent_->OnInsert(nb, context.depth_ + 1);            

            if(context.debug_) 
                parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)]);
              
        }
    }  
}

void BlackMoveGenerator::GetRookMoves(const Board & board, MGSearchContextualObject & context) {
    BBoard queens   = board.black_queen_;     
    BBoard rooks    = board.black_rook_ & ~context.bishop_pins_;

    BBoard pinned_rooks  = (rooks | queens) & context.rook_pins_;  
    BBoard nopin_rooks   = rooks & ~context.rook_pins_;  

    LoopBits(pinned_rooks) {
        Square x = LSquare(pinned_rooks);
        BBoard moves = Lookup::Rook(x, board.occ_) & context.moveable_squares_ & context.rook_pins_;     
        
        while(moves) {
            const Square to = PopBit(moves);
            if(1ULL << x & board.black_queen_) {
                const Board nb = UpdateQueenMove(board, x, to);
                parent_->OnInsert(nb, context.depth_ + 1); 
                 
                if(context.debug_) 
                    parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)]);
                    
            }
            else {
                const Board nb = UpdateRookMove(board, x, to);
                parent_->OnInsert(nb, context.depth_ + 1); 

                if(context.debug_) 
                    parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)]);
                       
            }
        }
    }

    LoopBits(nopin_rooks) {
        Square x = LSquare(nopin_rooks);

        BBoard moves = Lookup::Rook(x, board.occ_) & context.moveable_squares_; 

        while(moves) {                     
            const Square to = PopBit(moves); 

            const Board nb = UpdateRookMove(board, x, to);
            parent_->OnInsert(nb, context.depth_ + 1);            

            if(context.debug_) 
                parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)]);
                        
        }
    } 
}

void BlackMoveGenerator::GetQueenMoves(const Board & board, MGSearchContextualObject & context) {
    BBoard queens = board.black_queen_ & ~(context.rook_pins_ | context.bishop_pins_); 

    
    LoopBits(queens) {
        Square x = LSquare(queens);
        BBoard moves = Lookup::Queen(x, board.occ_) & context.moveable_squares_;         

        while(moves) {            
            Square to = PopBit(moves);           
           
            const Board nb = UpdateQueenMove(board, x, to);
            parent_->OnInsert(nb, context.depth_ + 1);
            
            if(context.debug_) 
                parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)]);
             
        } 
    } 
}

void BlackMoveGenerator::GetKingMoves(const Board & board, MGSearchContextualObject & context) {  
    Square x = LSquare(board.black_king_);
    BBoard moves = Lookup::King(x) &~ (context.kingban_ | board.black_);

    while(moves) {            
        Square to = PopBit(moves);    
      
        const Board nb = UpdateKingMove(board, x, to);
        parent_->OnInsert(nb, context.depth_ + 1);
      
        if(context.debug_) 
            parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)]);
         
    }
}

//check that the rook is there also!
void BlackMoveGenerator::GetCastlingMoves(const Board& board, MGSearchContextualObject & context) {    
    if( board.black_oo_ && 
        !((board.occ_ | context.kingban_) & BSCastleBlockedSquares) ) {        
        
        const Board nb = UpdateCastle00(board);
        parent_->OnInsert(nb, context.depth_ + 1);      

        if(context.debug_) 
            parent_->OnInsertDebug(board, nb, "e8g8");
        
    }

    if( board.black_ooo_ && 
        !(context.kingban_ & BLCastleBlockedSquares) &&
        !(board.occ_ & BLCastleNoOcc)) {        
        const Board nb = UpdateCastle000(board);
        parent_->OnInsert(nb, context.depth_ + 1);
      

         if(context.debug_) 
            parent_->OnInsertDebug(board, nb, "e8c8");
        
    }
}