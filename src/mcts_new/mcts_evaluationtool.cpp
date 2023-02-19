#include "mcts_evaluationtool.h"

#include <random>

using namespace MCTS_SIM;

MCTSSimulationStateGenerator::MCTSSimulationStateGenerator() {

}

std::pair<PositionStatus, Board>  MCTSSimulationStateGenerator::GetTransitions(const Board& board, const int& rselect) {
    rselect_ = rselect;
    entries_ = 0;
    bool undercheck;
    if(board.white_acts_) undercheck = wmgen_.ParseLegalMoves(board, 0);
    else undercheck = bmgen_.ParseLegalMoves(board, 0);

    return std::make_pair(PositionStatus(entries_ == 0 ? true : false, undercheck, entries_), rselect > entries_ ? any_ : selected_);
}

//random insertion
void MCTSSimulationStateGenerator::OnInsert(const Board& board, const int& depth) {
    if(rselect_ == entries_++) selected_ = board;
    else any_ = board;    
}

MCTSModelEvaluation::MCTSModelEvaluation() {

}

SimulationResult MCTSModelEvaluation::  SimulateGameplay(const Board& board, const OptionsDict& params) {

    Board current_state = board;
    
    //fake round to set starting range of move selection
    std::pair<PositionStatus, Board> s_t = GenerateStochasticTransition(board, rand() % 100);
    int rng_selection_ub = s_t.first.nmoves_;

    while(current_state.full_move_ < params.kMaxSimulationPly) {
        if(!HasMatingMaterial(current_state) || current_state.half_move_ >= 50) {
            return SimulationResult::DRAW;
        }

        std::pair<PositionStatus, Board> s_t = GenerateStochasticTransition(current_state, rand() % params.kRandSelectionUpperbound);

        if(s_t.first.terminal_) {
            if(s_t.first.nocheck_) return SimulationResult::DRAW;
            else {
                if(current_state.white_acts_) return SimulationResult::BLACKWIN;
                else return SimulationResult::WHITEWIN;
            }
        }

        current_state = s_t.second;
        rng_selection_ub = s_t.first.nmoves_;
    }

    return SimulationResult::UNDECISIVE;
}

std::pair<PositionStatus, Board> MCTSModelEvaluation::GenerateStochasticTransition(const Board& board, const int &rselect) {
    return state_generator_.GetTransitions(board, rselect);
    if(board.white_acts_) return white_state_generator_.ParseLegalMoves(board, rselect);
    else return black_state_generator_.ParseLegalMoves(board, rselect);

}