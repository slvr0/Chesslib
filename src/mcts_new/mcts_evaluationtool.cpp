#include "mcts_evaluationtool.h"

#include <random>

using namespace MCTS_SIM;

MCTSSimulationStateGenerator::MCTSSimulationStateGenerator() {

}

std::pair<Board, PositionStatus>  MCTSSimulationStateGenerator::GetTransitions(const Board& board, const int& rselect) {
    rselect_ = rselect;
    entries_ = 0;
    bool undercheck;
    if(board.white_acts_) undercheck = wmgen_.ParseLegalMoves(board, 0);
    else undercheck = bmgen_.ParseLegalMoves(board, 0);

    return std::make_pair(rselect > entries_ ? any_ : selected_, PositionStatus(entries_ == 0 ? true : false, undercheck));
}

//random insertion
void MCTSSimulationStateGenerator::OnInsert(const Board& board, const int& depth) {
    if(rselect_ == entries_++) selected_ = board;
    else any_ = board;    
}

MCTSModelEvaluation::MCTSModelEvaluation() {

}

SimulationResult MCTSModelEvaluation::SimulateGameplay(const Board& board, const OptionsDict& params) {

    Board current_state = board;
    while(current_state.full_move_ < params.kMaxSimulationPly) {
        if(!HasMatingMaterial(current_state) || current_state.half_move_ >= 50) {
            return SimulationResult::DRAW;
        }

        std::pair<Board, PositionStatus> s_t = GenerateStochasticTransition(current_state, rand() % params.kRandSelectionUpperbound);

        if(s_t.second.terminal_) {
            if(s_t.second.undercheck_) return SimulationResult::DRAW;
            else {
                if(current_state.white_acts_) return SimulationResult::BLACKWIN;
                else return SimulationResult::WHITEWIN;
            }
        }

        current_state = s_t.first;
    }

    return SimulationResult::UNDECISIVE;
}

std::pair<Board, PositionStatus>       MCTSModelEvaluation::GenerateStochasticTransition(const Board& board, const int &rselect) {
    return state_generator_.GetTransitions(board, rselect);
}

