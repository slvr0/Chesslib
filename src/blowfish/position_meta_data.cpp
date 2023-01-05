#include "position_meta_data.h"

void WhiteMetaDataRegister::InitMetaDataSearch(const Board& board) {
    rook_pins        = { 0x0 };
    bishop_pins      = { 0x0 };
    enp_target       = { 0x0 };
    checkmask        = { 0xffffffffffffffffull };
    kingban          = { 0x0 };
    kingattack       = { 0x0 }; //current moves for king
    enemy_kingattack = { 0x0 }; //current enemy king attacked squares
    kingmoves        = { 0x0 };
    ekingmoves       = { 0x0 };
    check_status     = { 0x0 };  


    RefreshMetaData(board);
}

void WhiteMetaDataRegister::RefreshMetaData(const Board & board) {  
    kingmoves =  Lookup::King(LSquare(board.white_king_)); 
    ekingmoves = Lookup::King(LSquare(board.black_king_));

    //Pawn checks
    {
        const BBoard pl = Black_Pawn_AttackLeft(board.black_pawn_ & Pawns_NotLeft());
        const BBoard pr = Black_Pawn_AttackRight(board.black_pawn_ & Pawns_NotRight());

        if (pl & board.white_king_) check_status = Black_Pawn_AttackRight(board.white_king_);
        else if (pr & board.white_king_) check_status = Black_Pawn_AttackLeft(board.white_king_);
        else check_status = 0xffffffffffffffffull;
    }

    //Knight checks
    {
        BBoard knightcheck = Lookup::Knight(LSquare(board.white_king_)) & board.black_knight_;
        if (knightcheck) check_status = knightcheck;
    }

    checkmask = check_status;


    const BBoard king   = board.white_king_;
    const BBoard eking  = board.black_king_;
    const BBoard kingsq = LSquare(king);

    //evaluate pinned pieces and checks from sliders
    {
        rook_pins = 0; 
        bishop_pins = 0;
        
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
      
        if(board.state_.enp_ != -1) {     
            enp_target = 1ULL << board.state_.enp_;

            const BBoard pawns = board.white_pawn_;
            const BBoard enemy_rook_queens = board.black_rook_ | board.black_queen_;
            const BBoard enp64 = 1ULL << board.state_.enp_;

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
                    BBoard AfterEPocc = board.occ_ & ~((enp_target >> 8) | EPLpawn);
                    if ((Lookup::Rook(kingsq, AfterEPocc) & WhiteEPRank()) & enemy_rook_queens) 
                        enp_target = 0x0;
                }
                if (EPRpawn) {
                    BBoard AfterEPocc = board.occ_ & ~((enp_target >> 8) | EPRpawn);
                    BoardConsoleGUI::PrintBoard(AfterEPocc); 
                    if ((Lookup::Rook(kingsq, AfterEPocc) & WhiteEPRank()) & enemy_rook_queens) 
                        enp_target = 0x0;
                }
            }           
        }
    }

    

    CalculateKingBan(board);
}

void WhiteMetaDataRegister::CalculateKingBan(const Board & board) {
    
    {
        
        BBoard knights = board.black_knight_;
        LoopBits(knights)
        {
            kingban |= Lookup::Knight(LSquare(knights));
        }
    }

    // Calculate Check from enemy pawns
    {
        
        const BBoard pl = Black_Pawn_AttackLeft(board.black_pawn_) & Pawns_NotLeft();
        const BBoard pr = Black_Pawn_AttackRight(board.black_pawn_) & Pawns_NotRight();
        kingban |= (pl | pr);
    }


    // Calculate Enemy Bishop
    {
        BBoard bishops = board.black_bishop_;
        LoopBits(bishops)
        {
            const Square sq = LSquare(bishops);                
            BBoard atk = Lookup::Bishop(sq, board.occ_);
            kingban |= atk;
        }
    }

    // Calculate Enemy Rook
    {
        BBoard rooks = (board.black_rook_ | board.black_queen_);
        LoopBits(rooks)
        {
            const Square sq = LSquare(rooks);
            BBoard atk = Lookup::Rook(sq, board.occ_);
            kingban |= atk;
        }
    }
}

