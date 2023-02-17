#include "mcts_evaluationtool.h"

#include <random>

using namespace MCTS_SIM;

MCTSSimulationStateGenerator::MCTSSimulationStateGenerator() {

}

std::pair<MoveList, bool>  MCTSSimulationStateGenerator::GetTransitions(const Board& board) {
    movelist_.clear();
    bool undercheck;
    if(board.white_acts_) undercheck = wmgen_.ParseLegalMoves(board, 0);
    else undercheck = bmgen_.ParseLegalMoves(board, 0);

    return std::make_pair(movelist_, undercheck);
}

//random insertion
void MCTSSimulationStateGenerator::OnInsert(const Board& board, const int& depth) {
    int randominsert = movelist_.empty() ? 0 : rand() % movelist_.size();
    movelist_.insert(movelist_.begin() + randominsert, board);
    
}

MCTSModelEvaluation::MCTSModelEvaluation() {

}

SimulationResult MCTSModelEvaluation::SimulateGameplay(const Board& board, const OptionsDict& params) {

    Board current_state = board;
    while(current_state.full_move_ < params.kMaxSimulationPly) {
        if(!HasMatingMaterial(current_state) || current_state.half_move_ >= 50) {
            return SimulationResult::DRAW;
        }

        std::pair<MoveList,bool> s_t = GetPossibleTransitions(current_state);

        if(s_t.first.empty()) {
            if(s_t.second) return SimulationResult::DRAW;
            else {
                if(current_state.white_acts_) return SimulationResult::BLACKWIN;
                else return SimulationResult::WHITEWIN;
            }
        }

        current_state = PickStochasticState(s_t.first);
    }

    return SimulationResult::UNDECISIVE;
}

std::pair<MoveList, bool>       MCTSModelEvaluation::GetPossibleTransitions(const Board& board) {
    return state_generator_.GetTransitions(board);
}

//random choice
Board                           MCTSModelEvaluation::PickStochasticState(MoveList movelist) {
    return movelist[rand() % movelist.size()];
}