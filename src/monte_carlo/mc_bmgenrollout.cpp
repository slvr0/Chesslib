#include "mc_bmgenrollout.h"

using namespace MCTS;

MGSearchContextualObject BlackRolloutMoveGenerator::RefreshMetaDataInternal(const Board &board)
{
    MGSearchContextualObject context;

    context.kingmoves_ = Lookup::King(LSquare(board.black_king_));
    context.ekingmoves_ = Lookup::King(LSquare(board.white_king_));

    // Pawn checks
    {
        const BBoard pl = White_Pawn_AttackLeft(board.white_pawn_ & Pawns_NotRight());
        const BBoard pr = White_Pawn_AttackRight(board.white_pawn_ & Pawns_NotLeft());

        context.kingban_ |= (pl | pr);

        if (pl & board.black_king_)
            context.check_status_ = Black_Pawn_AttackRight(board.black_king_);
        else if (pr & board.black_king_)
            context.check_status_ = Black_Pawn_AttackLeft(board.black_king_);
        else
            context.check_status_ = 0xffffffffffffffffull;
    }

    // Knight checks
    {
        BBoard knightcheck = Lookup::Knight(LSquare(board.black_king_)) & board.white_knight_;
        if (knightcheck)
            context.check_status_ = knightcheck;
    }

    context.checkmask_ = context.check_status_;

    const BBoard king = board.black_king_;
    const BBoard eking = board.white_king_;
    const BBoard kingsq = LSquare(king);

    // evaluate pinned pieces and checks from sliders
    {
        context.rook_pins_ = 0;
        context.bishop_pins_ = 0;

        if (Chess_Lookup::RookMask[kingsq] & (board.white_rook_ | board.white_queen_))
        {
            BBoard atkHV = Lookup::Rook(kingsq, board.occ_) & (board.white_rook_ | board.white_queen_);
            LoopBits(atkHV)
            {
                Square sq = LSquare(atkHV);
                CheckBySlider(kingsq, sq, context);
            }

            BBoard pinnersHV = Lookup::Rook_Xray(kingsq, board.occ_) & (board.white_rook_ | board.white_queen_);
            LoopBits(pinnersHV)
            {
                RegisterPinHorisontalVertical(kingsq, LSquare(pinnersHV), board, context);
            }
        }
        if (Chess_Lookup::BishopMask[kingsq] & (board.white_bishop_ | board.white_queen_))
        {
            BBoard atkD12 = Lookup::Bishop(kingsq, board.occ_) & (board.white_bishop_ | board.white_queen_);
            LoopBits(atkD12)
            {
                Square sq = LSquare(atkD12);
                CheckBySlider(kingsq, sq, context);
            }

            BBoard pinnersD12 = Lookup::Bishop_Xray(kingsq, board.occ_) & (board.white_bishop_ | board.white_queen_);
            LoopBits(pinnersD12)
            {
                RegisterPinDiagonal(kingsq, LSquare(pinnersD12), board, context);
            }
        }

        if (board.enp_ != -1)
        {
            context.enp_target_ = 1ULL << board.enp_;

            const BBoard pawns = board.black_pawn_;
            const BBoard enemy_rook_queens = board.white_rook_ | board.white_queen_;

            // Special Horizontal1 https://lichess.org/editor?fen=8%2F8%2F8%2F1K1pP1q1%2F8%2F8%2F8%2F8+w+-+-+0+1
            // Special Horizontal2 https://lichess.org/editor?fen=8%2F8%2F8%2F1K1pP1q1%2F8%2F8%2F8%2F8+w+-+-+0+1

            // King is on EP rank and enemy HV walker is on same rank

            // Remove enemy EP and own EP Candidate from OCC and check if Horizontal path to enemy Slider is open
            // Quick check: We have king - Enemy Slider - Own Pawn - and enemy EP on the same rank!
            if ((BlackEPRank() & king) && (BlackEPRank() & enemy_rook_queens) && (BlackEPRank() & pawns))
            {
                BBoard EPLpawn = pawns & Pawns_NotLeft() & (Black_Pawn_InvertLeft(context.enp_target_));   // Pawn that can EPTake to the left - overflow will not matter because 'Notleft'
                BBoard EPRpawn = pawns & Pawns_NotRight() & (Black_Pawn_InvertRight(context.enp_target_)); // Pawn that can EPTake to the right - overflow will not matter because 'NotRight'

                // invalidates EP from both angles
                if (EPLpawn)
                {
                    BBoard AfterEPocc = board.occ_ & ~((context.enp_target_ >> 8) | EPLpawn);
                    if ((Lookup::Rook(kingsq, AfterEPocc) & BlackEPRank()) & enemy_rook_queens)
                        context.enp_target_ = 0x0;
                }
                if (EPRpawn)
                {
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

RMGResult BlackRolloutMoveGenerator::ParseLegalMoves(const Board &board, const int &select_id, MGSearchContextualObject* rerun)
{
    MGSearchContextualObject context;
    if(rerun) {
        context = *rerun;
        delete(rerun);
    }
    else {
        context = RefreshMetaDataInternal(board);
    }

    context.enemy_or_void_ = ~board.black_;
    context.nocheck_ = (context.checkmask_ == 0xffffffffffffffffull);
    context.moveable_squares_ = context.enemy_or_void_ & context.checkmask_;
    
    int N = 0;

    { // king
        Square x = LSquare(board.black_king_);
        BBoard moves = Lookup::King(x) & ~(context.kingban_ | board.black_);

        while (moves)
        {
            Square to = PopBit(moves);
            if (select_increment(select_id, N++))
            {
                return MCTS::RMGResult(false, context.nocheck_, UpdateKingMove(board, x, to));
            }
        }
    } // eof king
    if(context.checkmask_) {
        { // pawns
            const BBoard pawns_lr = board.black_pawn_ & ~context.rook_pins_;
            const BBoard pawns_hv = board.black_pawn_ & ~context.bishop_pins_;

            BBoard pawn_capture_left = pawns_lr & Black_Pawn_InvertLeft(board.white_ & Pawns_NotLeft() & context.checkmask_);
            BBoard pawn_capture_right = pawns_lr & Black_Pawn_InvertRight(board.white_ & Pawns_NotRight() & context.checkmask_);

            // forward
            BBoard pawn_forward_1 = pawns_hv & Black_Pawn_Backward(~board.occ_); // no checkmask needed here? why? it comes later

            // push, pawn is on first rank, the slot two ranks forward is free, forward 1 is eligible
            uint64_t pawn_forward_2 = pawn_forward_1 & Black_Pawns_FirstRank() & Black_Pawn_Backward2(~board.occ_ & context.checkmask_);

            pawn_forward_1 &= Black_Pawn_Backward(context.checkmask_); // there is an optimization reason to move this after determining pawn forward 2,
            // with the following comment https://lichess.org/editor?fen=rnbpkbnr%2Fppp3pp%2F4pp2%2Fq7%2F8%2F3P4%2FPPP1PPPP%2FRNBQKBNR+w+KQkq+-+0+1
            // which i honestly dont fully understand

            Black_Pawns_PruneMove(pawn_forward_1, context.rook_pins_);
            Black_Pawns_PruneMove2(pawn_forward_2, context.rook_pins_);
            Black_Pawns_PruneLeft(pawn_capture_left, context.bishop_pins_);
            Black_Pawns_PruneRight(pawn_capture_right, context.bishop_pins_);

            if (context.enp_target_)
            {
                BBoard EPLpawn = pawns_lr & (Black_Pawn_InvertLeft(context.enp_target_ & Pawns_NotLeft()) & context.checkmask_ << 1);
                BBoard EPRpawn = pawns_lr & (Black_Pawn_InvertRight(context.enp_target_ & Pawns_NotRight()) & context.checkmask_ >> 1);

                if (EPLpawn | EPRpawn)
                {
                    Black_Pawn_PruneLeftEP(EPLpawn, context.bishop_pins_);
                    Black_Pawn_PruneRightEP(EPRpawn, context.bishop_pins_);

                    if (EPLpawn)
                    {
                        const Bit pos = PopBit(EPLpawn);
                        const Square to = Black_Pawn_AttackLeft(pos);

                        const Board current = UpdatePawnEnpassaint(board, pos, to, context.enp_target_ << 8);
                        if (select_increment(select_id, N++))
                        {
                            return MCTS::RMGResult(false, context.nocheck_, current);
                        }
                    }

                    if (EPRpawn)
                    {
                        const Bit pos = PopBit(EPRpawn);
                        const Square to = Black_Pawn_AttackRight(pos);

                        const Board current = UpdatePawnEnpassaint(board, pos, to, context.enp_target_ << 8);
                        if (select_increment(select_id, N++))
                        {
                            return MCTS::RMGResult(false, context.nocheck_, current);
                        }
                    }
                }
            }

            // Handle PROMOTION and make moves from all legal pawn moves pruned

            if ((pawn_capture_left | pawn_capture_right | pawn_forward_1) & Black_Pawns_LastRank())
            {
                uint64_t Promote_Left = pawn_capture_left & Black_Pawns_LastRank();
                uint64_t Promote_Right = pawn_capture_right & Black_Pawns_LastRank();
                uint64_t Promote_Move = pawn_forward_1 & Black_Pawns_LastRank();

                uint64_t NoPromote_Left = pawn_capture_left & ~Black_Pawns_LastRank();
                uint64_t NoPromote_Right = pawn_capture_right & ~Black_Pawns_LastRank();
                uint64_t NoPromote_Move = pawn_forward_1 & ~Black_Pawns_LastRank();

                // we treat promo transitions different
                while (Promote_Left)
                {
                    const Bit pos = PopBit(Promote_Left);
                    const Square to = Black_Pawn_AttackLeft(pos);

                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPromotion(board, PieceType::KNIGHT, pos, to));
                    }
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPromotion(board, PieceType::BISHOP, pos, to));
                    }
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPromotion(board, PieceType::ROOK, pos, to));
                    }
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPromotion(board, PieceType::QUEEN, pos, to));
                    }
                }
                while (Promote_Right)
                {
                    const Bit pos = PopBit(Promote_Right);
                    const Square to = Black_Pawn_AttackRight(pos);

                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPromotion(board, PieceType::KNIGHT, pos, to));
                    }
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPromotion(board, PieceType::BISHOP, pos, to));
                    }
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPromotion(board, PieceType::ROOK, pos, to));
                    }
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPromotion(board, PieceType::QUEEN, pos, to));
                    }
                }
                while (Promote_Move)
                {
                    const Bit pos = PopBit(Promote_Move);
                    const Square to = Black_Pawn_Forward(pos);

                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPromotion(board, PieceType::KNIGHT, pos, to));
                    }
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPromotion(board, PieceType::BISHOP, pos, to));
                    }
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPromotion(board, PieceType::ROOK, pos, to));
                    }
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPromotion(board, PieceType::QUEEN, pos, to));
                    }
                }
                while (NoPromote_Left)
                {
                    const Bit pos = PopBit(NoPromote_Left);
                    const Square to = Black_Pawn_AttackLeft(pos);

                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnCapture(board, pos, to));
                    }
                }
                while (NoPromote_Right)
                {
                    const Bit pos = PopBit(NoPromote_Right);
                    const Square to = Black_Pawn_AttackRight(pos);

                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnCapture(board, pos, to));
                    }
                }
                while (NoPromote_Move)
                {
                    const Bit pos = PopBit(NoPromote_Move);
                    const Square to = Black_Pawn_Forward(pos);

                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnMove(board, pos, to));
                    }
                }
                while (pawn_forward_2)
                {
                    const Bit pos = PopBit(pawn_forward_2);
                    const Square to = Black_Pawn_Forward2(pos);

                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPush(board, pos, to));
                    }
                }
            }
            else
            {
                while (pawn_capture_left)
                {
                    const Bit pos = PopBit(pawn_capture_left);
                    const Square to = Black_Pawn_AttackLeft(pos);

                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnCapture(board, pos, to));
                    }
                }
                while (pawn_capture_right)
                {
                    const Bit pos = PopBit(pawn_capture_right);
                    const Square to = Black_Pawn_AttackRight(pos);
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnCapture(board, pos, to));
                    }
                }
                while (pawn_forward_1)
                {
                    const Bit pos = PopBit(pawn_forward_1);
                    const Square to = Black_Pawn_Forward(pos);

                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnMove(board, pos, to));
                    }
                }
                while (pawn_forward_2)
                {
                    const Bit pos = PopBit(pawn_forward_2);
                    const Square to = Black_Pawn_Forward2(pos);

                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdatePawnPush(board, pos, to));
                    }
                }
            }
        } // eof pawns

        { // knights
            BBoard knights = board.black_knight_ & (~(context.rook_pins_ | context.bishop_pins_));
            LoopBits(knights)
            {
                Square x = LSquare(knights);
                BBoard moves = Lookup::Knight(x) & context.moveable_squares_;

                while (moves)
                {
                    Square to = PopBit(moves);
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdateKnightMove(board, x, to));
                    }
                }
            }

        } // eof knights

        { // bishops
            BBoard queens = board.black_queen_;
            BBoard bishops = board.black_bishop_ & ~context.rook_pins_;
            BBoard pinned_bishops = (bishops | queens) & context.bishop_pins_;
            BBoard nopin_bishops = bishops & ~context.bishop_pins_;

            LoopBits(pinned_bishops)
            {
                Square x = LSquare(pinned_bishops);
                BBoard moves = Lookup::Bishop(x, board.occ_) & context.moveable_squares_ & context.bishop_pins_;

                if ((1ULL << x) & board.black_queen_)
                {
                    while (moves)
                    {
                        Square to = PopBit(moves);
                        if (select_increment(select_id, N++))
                        {
                            return MCTS::RMGResult(false, context.nocheck_, UpdateQueenMove(board, x, to));
                        }
                    }
                }
                else
                {
                    while (moves)
                    {
                        Square to = PopBit(moves);
                        if (select_increment(select_id, N++))
                        {
                            return MCTS::RMGResult(false, context.nocheck_, UpdateBishopMove(board, x, to));
                        }
                    }
                }
            }
            LoopBits(nopin_bishops)
            {
                Square x = LSquare(nopin_bishops);
                BBoard moves = Lookup::Bishop(x, board.occ_) & context.moveable_squares_;
                while (moves)
                {
                    Square to = PopBit(moves);
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdateBishopMove(board, x, to));
                    }
                }
            }
        } // eof bishops

        { // rooks
            BBoard queens       = board.black_queen_;
            BBoard rooks        = board.black_rook_ & ~context.bishop_pins_;
            BBoard pinned_rooks = (rooks | queens) & context.rook_pins_;
            BBoard nopin_rooks  = rooks & ~context.rook_pins_;

            LoopBits(pinned_rooks)
            {
                Square x = LSquare(pinned_rooks);
                BBoard moves = Lookup::Rook(x, board.occ_) & context.moveable_squares_ & context.rook_pins_;

                while (moves)
                {
                    const Square to = PopBit(moves);
                    if (1ULL << x & board.white_queen_)
                    {                   
                        if (select_increment(select_id, N++))
                        {
                            return MCTS::RMGResult(false, context.nocheck_, UpdateQueenMove(board, x, to));
                        }
                    }
                    else
                    {
                        if (select_increment(select_id, N++))
                        {
                            return MCTS::RMGResult(false, context.nocheck_, UpdateRookMove(board, x, to));
                        }
                    }
                }
            }
            LoopBits(nopin_rooks)
            {
                Square x = LSquare(nopin_rooks);
                BBoard moves = Lookup::Rook(x, board.occ_) & context.moveable_squares_;

                while (moves)
                {
                    const Square to = PopBit(moves);
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdateRookMove(board, x, to));
                    }
                }
            }
        } // eof rooks

        { // queens
            BBoard queens = board.black_queen_ & ~(context.rook_pins_ | context.bishop_pins_);

            LoopBits(queens)
            {
                Square x = LSquare(queens);
                BBoard moves = Lookup::Queen(x, board.occ_) & context.moveable_squares_;

                while (moves)
                {
                    Square to = PopBit(moves);
                    if (select_increment(select_id, N++))
                    {
                        return MCTS::RMGResult(false, context.nocheck_, UpdateQueenMove(board, x, to));
                    }
                }
            }
        } // eof queens
    }
    if(context.nocheck_) {
        { // castle
            if (board.black_oo_ &&
                !((board.occ_ | context.kingban_) & BSCastleBlockedSquares))
            {
                if (select_increment(select_id, N++))
                {
                    return MCTS::RMGResult(false, context.nocheck_, UpdateCastle00(board));
                }
            }
            if (board.black_ooo_ &&
                !(context.kingban_ & BLCastleBlockedSquares) &&
                !(board.occ_ & BLCastleNoOcc))
            {
                if (select_increment(select_id, N++))
                {
                    return MCTS::RMGResult(false, context.nocheck_, UpdateCastle000(board));
                }
            }
        } // eof castle
    }

    if(select_id > N) { //we missed it, re run and take first available move
        return ParseLegalMoves(board, 0, new MGSearchContextualObject(context));

    }
    else if(N == 0){ // false state
        return MCTS::RMGResult(true, context.nocheck_, board);
    }
}