#include "mcts_defines.h"

#include <iostream>

void MCTSNodeTreeStatistics::UpdateWithInsertion(const int& at_depth) {
    ++nentries_;
    ++nentries_per_d_[at_depth];
}

//more complex, TBD
void MCTSNodeTreeStatistics::UpdateAfterSlicing() {

}

int MCTSNodeTreeStatistics::GetEntries() const {
    return nentries_;
}

std::string MCTSNodeTreeStatistics::CurrentTree() const {
    std::string res = "Entries : " + std::to_string(nentries_) + "\n";
    for(int i = 0; i < nentries_per_d_.size(); ++i) {
        if(nentries_per_d_[i] == 0) break;
        res += std::to_string(i) + "|" + std::to_string(nentries_per_d_[i]) + "\n";
    }
    return res;
}