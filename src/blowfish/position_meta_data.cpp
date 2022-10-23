#include "position_meta_data.h"


void MetaDataRegister::InitMetaDataSearch(const Board& board) {  

        const bool white = board.state_.white_acts_;
        const bool enemy = !white;     

        kingmoves =  Lookup::King(LSquare(GetActiveKing(board))); //there is room for improvements in these algorithms, many duplicate searches
        ekingmoves = Lookup::King(LSquare(GetEnemyKing(board)));

        //Pawn checks
        {
            const BBoard pl = Pawn_AttackLeft(GetEnemyPawns(board) & Pawns_NotLeft(), enemy);
            const BBoard pr = Pawn_AttackRight(GetEnemyPawns(board) & Pawns_NotRight(), enemy);

            if (pl & GetActiveKing(board)) check_status = Pawn_AttackRight(GetActiveKing(board), white);
            else if (pr & GetActiveKing(board)) check_status = Pawn_AttackLeft(GetActiveKing(board), white);
            else check_status = 0xffffffffffffffffull;
        }

        //Knight checks
        {
            BBoard knightcheck = Lookup::Knight(LSquare(GetActiveKing(board))) & GetEnemyKnights(board);
            if (knightcheck) check_status = knightcheck;
        }

        checkmask = check_status;
}


void MetaDataRegister::RefreshMetaData(const Board & board) {

    InitMetaDataSearch(board);

    const BBoard king   = GetActiveKing(board);
    const BBoard eking  = GetEnemyKing(board);
    const BBoard kingsq = LSquare(king);

    //evaluate pinned pieces and checks from sliders
    {
        rook_pins = 0; 
        bishop_pins = 0;
        
        if (Chess_Lookup::RookMask[kingsq] & GetEnemyRookQueen(board))
        {
            BBoard atkHV = Lookup::Rook(kingsq, board.occ_) & GetEnemyRookQueen(board);
            LoopBits(atkHV) {
                Square sq = LSquare(atkHV);
                CheckBySlider(kingsq, sq);
            }

            BBoard pinnersHV = Lookup::Rook_Xray(kingsq, board.occ_) & GetEnemyRookQueen(board);
            LoopBits(pinnersHV)
            {                
                RegisterPinHorisontalVertical(kingsq, LSquare(pinnersHV), board);
            }
        }
        if (Chess_Lookup::BishopMask[kingsq] & GetEnemyBishopQueen(board)) {
            BBoard atkD12 = Lookup::Bishop(kingsq, board.occ_) & GetEnemyBishopQueen(board);
            LoopBits(atkD12) {
                Square sq = LSquare(atkD12);
                CheckBySlider(kingsq, sq);
            }

            BBoard pinnersD12 = Lookup::Bishop_Xray(kingsq, board.occ_) & GetEnemyBishopQueen(board);
            LoopBits(pinnersD12)
            {               
                RegisterPinDiagonal(kingsq, LSquare(pinnersD12), board);
            }
        }

/*         if constexpr (status.HasEPPawn)
        {
            RegisterPinEP<status>(kingsq, king, EnemyRookQueen<white>(brd), brd);
        } */
    }















}