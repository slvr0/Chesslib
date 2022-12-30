#include "position_meta_data.h"

void WhiteMetaDataRegister::InitMetaDataSearch(const Board& board) {
    rook_pins        = { };
    bishop_pins      = { };
    enp_target       = { };
    checkmask        = { 0xffffffffffffffffull };
    kingban          = { };
    kingattack       = { }; //current moves for king
    enemy_kingattack = { }; //current enemy king attacked squares
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

