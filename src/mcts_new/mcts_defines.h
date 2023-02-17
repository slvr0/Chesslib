#ifndef MCTS_DEFINES_H
#define MCTS_DEFINES_H

#include <iostream>
#include <vector>

enum class SimulationResult {
    UNDECISIVE = 0, 
    WHITEWIN = 1,
    BLACKWIN = 2,
    DRAW = 3
};

class MCTSNodeTreeStatistics {
public :
    MCTSNodeTreeStatistics(const int& max_eval_depth) {
        for(int k = 0 ; k < max_eval_depth; ++k) nentries_per_d_.push_back(0);
    }

    void UpdateWithInsertion(const int& at_depth);
    void UpdateAfterSlicing();

    std::string CurrentTree() const;
    int GetEntries() const;

private:
    int                 nentries_ = 0;
    std::vector<int>    nentries_per_d_;
};

//hyperparams
class OptionsDict {
public:
    static constexpr uint16_t kMaxSimulationPly = 150; 


};

#endif