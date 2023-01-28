#include "wmovegen.h"

#include "move_generator.h"
MGSearchContextualObject WhiteMoveGenerator::RefreshMetaDataInternal(const Board& board) {
    MGSearchContextualObject context;
    
    context.kingmoves_ =  Lookup::King(LSquare(board.white_king_)); 
    context.ekingmoves_ = Lookup::King(LSquare(board.black_king_));


    //Pawn checks
    {
        const BBoard pl = Black_Pawn_AttackLeft(board.black_pawn_ & Pawns_NotLeft());
        const BBoard pr = Black_Pawn_AttackRight(board.black_pawn_ & Pawns_NotRight());

 

        context.kingban_ |= (pl | pr);

        if (pl & board.white_king_) context.check_status_ = Black_Pawn_AttackRight(board.white_king_);
        else if (pr & board.white_king_) context.check_status_ = Black_Pawn_AttackLeft(board.white_king_);
        else context.check_status_ = 0xffffffffffffffffull;
    }

    //Knight checks
    {
        BBoard knightcheck = Lookup::Knight(LSquare(board.white_king_)) & board.black_knight_;
        if (knightcheck) context.check_status_ = knightcheck;
    }

    context.checkmask_ = context.check_status_;


    const BBoard king   = board.white_king_;
    const BBoard eking  = board.black_king_;
    const BBoard kingsq = LSquare(king);

    //evaluate pinned pieces and checks from sliders
    {
        context.rook_pins_ = 0; 
        context.bishop_pins_ = 0;
        
        if (Chess_Lookup::RookMask[kingsq] & (board.black_rook_ | board.black_queen_))
        {
            BBoard atkHV = Lookup::Rook(kingsq, board.occ_) & (board.black_rook_ | board.black_queen_);
            LoopBits(atkHV) {
                Square sq = LSquare(atkHV);
                CheckBySlider(kingsq, sq, context);
            }

            BBoard pinnersHV = Lookup::Rook_Xray(kingsq, board.occ_) & (board.black_rook_ | board.black_queen_);
            LoopBits(pinnersHV)
            {                
                RegisterPinHorisontalVertical(kingsq, LSquare(pinnersHV), board, context);
            }
        }
        if (Chess_Lookup::BishopMask[kingsq] & (board.black_bishop_ | board.black_queen_)) {
            BBoard atkD12 = Lookup::Bishop(kingsq, board.occ_) & (board.black_bishop_ | board.black_queen_);
            LoopBits(atkD12) {
                Square sq = LSquare(atkD12);
                CheckBySlider(kingsq, sq, context);
            }

            BBoard pinnersD12 = Lookup::Bishop_Xray(kingsq, board.occ_) & (board.black_bishop_ | board.black_queen_);
            LoopBits(pinnersD12)
            {               
                RegisterPinDiagonal(kingsq, LSquare(pinnersD12), board, context);
            }
        } 
      
        if(board.enp_ != -1) {     
            context.enp_target_ = 1ULL << board.enp_;

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
                    BBoard AfterEPocc = board.occ_ & ~((context.enp_target_ >> 8) | EPLpawn);
                    if ((Lookup::Rook(kingsq, AfterEPocc) & WhiteEPRank()) & enemy_rook_queens) 
                        context.enp_target_ = 0x0;
                }
                if (EPRpawn) {
                    BBoard AfterEPocc = board.occ_ & ~((context.enp_target_ >> 8) | EPRpawn);                    
                    if ((Lookup::Rook(kingsq, AfterEPocc) & WhiteEPRank()) & enemy_rook_queens) 
                        context.enp_target_ = 0x0;
                }
            }           
        }
    } 
       
    {        
        BBoard knights = board.black_knight_;
        LoopBits(knights)
        {
            context.kingban_ |= Lookup::Knight(LSquare(knights));
        }
    }

    // Calculate Enemy Bishop
    {
        BBoard bishops = board.black_bishop_ | board.black_queen_;
        LoopBits(bishops)
        {
            const Square sq = LSquare(bishops);                
            BBoard atk = Lookup::Bishop(sq, board.occ_);
            context.kingban_ |= atk;
        }
    }

    // Calculate Enemy Rook

    {
        BBoard rooks = (board.black_rook_ | board.black_queen_);
        LoopBits(rooks)
        {
            const Square sq = LSquare(rooks);
            BBoard atk = Lookup::Rook(sq, board.occ_);
            context.kingban_ |= atk;
        }
    }

    return context;
}

WhiteMoveGenerator::WhiteMoveGenerator(MoveGeneratorHeader* parent) : 
    parent_(parent)   
{ 
   
}

void WhiteMoveGenerator::ParseLegalMoves(const Board& board, const int& depth) {

    MGSearchContextualObject context_object = RefreshMetaDataInternal(board);

    context_object.depth_ = depth;      
    context_object.enemy_or_void_ = ~board.white_;
    context_object.nocheck_ = (context_object.checkmask_ == 0xffffffffffffffffull); 
    context_object.moveable_squares_   = context_object.enemy_or_void_ & context_object.checkmask_;

    //META DATA prepared.

    //if n checkers > 1 only traverse this
    GetKingMoves(board, context_object);

    //else proceed these/
    GetPawnMoves(board, context_object);
    GetKnightMoves(board, context_object);
    GetBishopMoves(board, context_object);    
    GetRookMoves(board, context_object); 
    GetQueenMoves(board, context_object);

    if(context_object.nocheck_) GetCastlingMoves(board, context_object);     
}

void WhiteMoveGenerator::GetPawnMoves(const Board & board, MGSearchContextualObject & context) {  
    const BBoard pawns_lr = board.white_pawn_ &~ context.rook_pins_;
    const BBoard pawns_hv = board.white_pawn_ &~ context.bishop_pins_;

    BBoard pawn_capture_left    = pawns_lr & White_Pawn_InvertLeft(board.black_ & Pawns_NotLeft() & context.checkmask_);
    BBoard pawn_capture_right   = pawns_lr & White_Pawn_InvertRight(board.black_ & Pawns_NotRight() & context.checkmask_);    

    //forward
    BBoard pawn_forward_1 = pawns_hv & White_Pawn_Backward(~board.occ_); // no checkmask needed here? why? it comes later

    //push, pawn is on first rank, the slot two ranks forward is free, forward 1 is eligible
    uint64_t pawn_forward_2 = pawn_forward_1 & White_Pawns_FirstRank() & White_Pawn_Backward2(~board.occ_ & context.checkmask_);

    pawn_forward_1 &= White_Pawn_Backward(context.checkmask_); //there is an optimization reason to move this after determining pawn forward 2, 
    //with the following comment https://lichess.org/editor?fen=rnbpkbnr%2Fppp3pp%2F4pp2%2Fq7%2F8%2F3P4%2FPPP1PPPP%2FRNBQKBNR+w+KQkq+-+0+1
    //which i honestly dont fully understand

    White_Pawns_PruneMove(pawn_forward_1,       context.rook_pins_);
    White_Pawns_PruneMove2(pawn_forward_2,      context.rook_pins_);
    White_Pawns_PruneLeft(pawn_capture_left,    context.bishop_pins_);
    White_Pawns_PruneRight(pawn_capture_right,  context.bishop_pins_);
   
    if (context.enp_target_) {    
        BBoard EPLpawn = pawns_lr & Pawns_NotLeft()  & (White_Pawn_InvertLeft(context.enp_target_  & context.checkmask_)); 
        BBoard EPRpawn = pawns_lr & Pawns_NotRight() & (White_Pawn_InvertRight(context.enp_target_ & context.checkmask_));  
        
        if (EPLpawn | EPRpawn) //Todo: bench if slower or faster
        {
            White_Pawn_PruneLeftEP(EPLpawn,     context.bishop_pins_);
            White_Pawn_PruneRightEP(EPRpawn,    context.bishop_pins_);

            /*

            //enumerate these...
            if(EPLpawn) { const Bit pos = PopBit(EPLpawn);     const Square to = White_Pawn_AttackLeft(pos); 
            std::cout << "P(EP LEFT) || " <<  notations[LSquare(pos)] << " || " << notations[LeastBit(to)] << std::endl;} // add legal EP move
            if(EPRpawn) { const Bit pos = PopBit(EPRpawn);    const Square to = White_Pawn_AttackRight(pos);
            std::cout << "P(EP RIGHT) || " <<  notations[LSquare(pos)] << " || " << notations[LeastBit(to)] << std::endl; } // add legal EP move

            */
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
            const Board nb1 = UpdatePawnPromotion(board, PieceType::KNIGHT, pos, to);
            parent_->OnInsert(nb1, context.depth_ + 1);
            const Board nb2 = UpdatePawnPromotion(board, PieceType::BISHOP, pos, to);
            parent_->OnInsert(nb2, context.depth_ + 1);
            const Board nb3 = UpdatePawnPromotion(board, PieceType::ROOK, pos, to);
            parent_->OnInsert(nb3, context.depth_ + 1);
            const Board nb4 = UpdatePawnPromotion(board, PieceType::QUEEN, pos, to);  
            parent_->OnInsert(nb4, context.depth_ + 1);   

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb1, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Promote Knight");
                parent_->OnInsertDebug(board, nb2, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Promote Bishop");
                parent_->OnInsertDebug(board, nb3, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Promote Rook");
                parent_->OnInsertDebug(board, nb4, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Promote Queen");       
            #endif      
        }
        while (Promote_Right)   { 
            const Bit pos = PopBit(Promote_Right);    const Square to = White_Pawn_AttackRight(pos);
            const Board nb1 = UpdatePawnPromotion(board, PieceType::KNIGHT, pos, to);
            parent_->OnInsert(nb1, context.depth_ + 1);
            const Board nb2 = UpdatePawnPromotion(board, PieceType::BISHOP, pos, to);
            parent_->OnInsert(nb2, context.depth_ + 1);
            const Board nb3 = UpdatePawnPromotion(board, PieceType::ROOK, pos, to);
            parent_->OnInsert(nb3, context.depth_ + 1);
            const Board nb4 = UpdatePawnPromotion(board, PieceType::QUEEN, pos, to); 
            parent_->OnInsert(nb4, context.depth_ + 1);

            #ifdef _DEBUG
                parent_->OnInsertDebug(board, nb1, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Promote Knight");
                parent_->OnInsertDebug(board, nb2, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Promote Bishop");
                parent_->OnInsertDebug(board, nb3, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Promote Rook");
                parent_->OnInsertDebug(board, nb4, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Promote Queen");  
            #endif
        }
        while (Promote_Move)    { 
            const Bit pos = PopBit(Promote_Move);     const Square to = White_Pawn_Forward(pos);
            const Board nb1 = UpdatePawnPromotion(board, PieceType::KNIGHT, pos, to);
            parent_->OnInsert(nb1, context.depth_ + 1);
            const Board nb2 = UpdatePawnPromotion(board, PieceType::BISHOP, pos, to);
            parent_->OnInsert(nb2, context.depth_ + 1);
            const Board nb3 = UpdatePawnPromotion(board, PieceType::ROOK, pos, to);
            parent_->OnInsert(nb3, context.depth_ + 1);
            const Board nb4 = UpdatePawnPromotion(board, PieceType::QUEEN, pos, to); 
            parent_->OnInsert(nb4, context.depth_ + 1);

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb1, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Promote Knight");
                parent_->OnInsertDebug(board, nb2, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Promote Bishop");
                parent_->OnInsertDebug(board, nb3, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Promote Rook");
                parent_->OnInsertDebug(board, nb4, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Promote Queen");   
            #endif 
        }
        while (NoPromote_Left)  { 
            const Bit pos = PopBit(NoPromote_Left);   const Square to = White_Pawn_AttackLeft(pos);
            const Board nb = UpdatePawnCapture(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn capture Left");
            #endif
            
        }
        while (NoPromote_Right) { 
            const Bit pos = PopBit(NoPromote_Right);  const Square to = White_Pawn_AttackRight(pos);
            const Board nb = UpdatePawnCapture(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)]  + notations[LSquare(to)] + " Pawn capture Right");
            #endif
        }
        while (NoPromote_Move)  { 
            const Bit pos = PopBit(NoPromote_Move);   const Square to = White_Pawn_Forward(pos);
            const Board nb =UpdatePawnMove(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Move");
            #endif            
            
        }
        while (pawn_forward_2)  { 
            const Bit pos = PopBit(pawn_forward_2);   const Square to = White_Pawn_Forward2(pos);
            const Board nb =UpdatePawnPush(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)]  + notations[LSquare(to)] + " Pawn Push");
            #endif           
        }
    }
    else { 
        while (pawn_capture_left)  { 
            const Bit pos = PopBit(pawn_capture_left);  const Square to = White_Pawn_AttackLeft(pos);          
            const Board nb =UpdatePawnCapture(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);           

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn capture Left");
            #endif
        }
        while (pawn_capture_right) { 
            const Bit pos = PopBit(pawn_capture_right); const Square to = White_Pawn_AttackRight(pos);
            const Board nb =UpdatePawnCapture(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);     

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)]  + notations[LSquare(to)] + " Pawn capture Right");
            #endif
        }
        while (pawn_forward_1)     { 
            const Bit pos = PopBit(pawn_forward_1);     const Square to = White_Pawn_Forward(pos);
            const Board nb = UpdatePawnMove(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Move");
            #endif 
        }
        while (pawn_forward_2)     { 
            const Bit pos = PopBit(pawn_forward_2);     const Square to = White_Pawn_Forward2(pos);           
            const Board nb = UpdatePawnPush(board, pos, to);
            parent_->OnInsert(nb, context.depth_ + 1);            

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb, notations[LSquare(pos)] + notations[LSquare(to)] + " Pawn Push");
            #endif
        }
    }    
}

void WhiteMoveGenerator::GetKnightMoves(const Board & board, MGSearchContextualObject & context) {
    BBoard knights = board.white_knight_ & (~ (context.rook_pins_ | context.bishop_pins_)); //a pinned knight can never move!

    LoopBits(knights) {
        Square x = LSquare(knights);    

        BBoard moves = Lookup::Knight(x) & context.moveable_squares_;
     
        while(moves) {            
            Square to = PopBit(moves);        

            const Board nb = UpdateKnightMove(board, x, to);
            parent_->OnInsert(nb, context.depth_ + 1);

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)] + " Knight Move");
            #endif    
        }
    } 
  
}

void WhiteMoveGenerator::GetBishopMoves(const Board & board, MGSearchContextualObject & context) {

    BBoard queens = board.white_queen_; 

    BBoard bishops = board.white_bishop_ &~ context.rook_pins_; //including non pinned bishops (HV) 
    
    BBoard pinned_bishops = (bishops | queens) & context.bishop_pins_; //including pinned queens diagonally here.
    BBoard nopin_bishops = bishops & ~context.bishop_pins_; 
    
    LoopBits(pinned_bishops) {
        Square x = LSquare(pinned_bishops);

        BBoard moves = Lookup::Bishop(x, board.occ_) & context.moveable_squares_ & context.bishop_pins_; 

        if(1ULL << x & queens) {
            
            while(moves)  {
                Square to = PopBit(moves); 
            
                const Board nb = UpdateQueenMove(board, x, to);
                parent_->OnInsert(nb, context.depth_ + 1);               

                #ifdef _DEBUG 
                    parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)] + " Queen Move");
                #endif  
            }
        }
        else {
             while(moves)  {
                Square to = PopBit(moves); 
                
                const Board nb = UpdateBishopMove(board, x, to);
                parent_->OnInsert(nb, context.depth_ + 1);                

                #ifdef _DEBUG 
                    parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)] + " Bishop Move");
                #endif  
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

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)] + " Bishop Move");
            #endif  
        }
    } 
}

void WhiteMoveGenerator::GetRookMoves(const Board & board, MGSearchContextualObject & context) {
    BBoard pinned_rooks  = (board.white_rook_ | board.white_queen_) & context.rook_pins_;
    BBoard rooks = board.white_rook_ & ~context.rook_pins_;

    LoopBits(pinned_rooks) {
        Square x = LSquare(pinned_rooks);
        BBoard moves = Lookup::Rook(x, board.occ_) & context.moveable_squares_ & context.rook_pins_; 

        while(moves) {
            const Square to = PopBit(moves);
            if(1ULL << x & board.white_queen_) {
                const Board nb = UpdateQueenMove(board, x, to);
                parent_->OnInsert(nb, context.depth_ + 1); 
                 
                #ifdef _DEBUG 
                    parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)] + " Queen Move");
                #endif    
            }
            else {
                const Board nb = UpdateRookMove(board, x, to);
                parent_->OnInsert(nb, context.depth_ + 1); 

                #ifdef _DEBUG 
                    parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)] + " Rook Move");
                #endif       
            }
        }

    }

    LoopBits(rooks) {
        Square x = LSquare(rooks);

        BBoard moves = Lookup::Rook(x, board.occ_) & context.moveable_squares_; 

        while(moves) {            
            const Square to = PopBit(moves); 

            const Board nb = UpdateRookMove(board, x, to);
            parent_->OnInsert(nb, context.depth_ + 1);            

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)] + " Rook Move");
            #endif            
        }
    }                                            
}

void WhiteMoveGenerator::GetQueenMoves(const Board & board, MGSearchContextualObject & context) {
    BBoard queens = board.white_queen_; 

    LoopBits(queens) {
        Square x = LSquare(queens);
        BBoard moves = Lookup::Queen(x, board.occ_) & context.moveable_squares_; 

        while(moves) {            
            Square to = PopBit(moves);           
        
            const Board nb = UpdateQueenMove(board, x, to);
            parent_->OnInsert(nb, context.depth_ + 1);       

            #ifdef _DEBUG 
                parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)] + " Queen Move");
            #endif   
        } 
    } 
}

void WhiteMoveGenerator::GetKingMoves(const Board & board, MGSearchContextualObject & context) {  
    m_assert(board.white_king_ != 0,  "White has no king");
    
    Square x = LSquare(board.white_king_);
    BBoard moves = Lookup::King(x) &~ (context.kingban_ | board.white_);

    while(moves) {            
        Square to = PopBit(moves);    
        
        const Board nb = UpdateKingMove(board, x, to);
        parent_->OnInsert(nb, context.depth_ + 1);        

        #ifdef _DEBUG 
            parent_->OnInsertDebug(board, nb, notations[x] + notations[LSquare(to)] + " King Move");
        #endif   
    }
}

void WhiteMoveGenerator::GetCastlingMoves(const Board& board, MGSearchContextualObject & context) {    
    if(board.white_oo_ && !((board.occ_ | context.kingban_) & wRCastleInterferenceSquares)) {       
       
        const Board nb = UpdateCastle00(board);
        parent_->OnInsert(nb, context.depth_ + 1);       

        #ifdef _DEBUG 
            parent_->OnInsertDebug(board, nb, "Castle King Side");
        #endif 
    }

    if(board.white_ooo_ && !((board.occ_ | context.kingban_) & wLCastleInterferenceSquares)) {
      
        const Board nb = UpdateCastle000(board);
        parent_->OnInsert(nb, context.depth_ + 1);   

        #ifdef _DEBUG 
            parent_->OnInsertDebug(board, nb, "Castle Queen Side");
        #endif 
    }
}