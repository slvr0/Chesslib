#pragma once


//holds meta data and config params for tree searches
//weights in UCB , node choice param A (for MCTSSolver v + A / sqrt(N)) etc

namespace MCTS {
    class MCConfigurationParams {
    public: 
        static constexpr int number_of_mcthreads = 1;
        static constexpr int nmax_threadsearches = 200000;

        static constexpr float nwbias = 5;
        static constexpr float ndbias = 1;
        static constexpr float exploration_factor = 2.0;


    };

}