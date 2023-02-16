#pragma once


//holds meta data and config params for tree searches
//weights in UCB , node choice param A (for MCTSSolver v + A / sqrt(N)) etc

namespace MCTS {
    class MCConfigurationParams {
    public: 
        static constexpr int number_of_mcthreads = 1;
        static constexpr int nmax_threadsearches = 1000000;

        static constexpr float nwbias = 1;
        static constexpr float ndbias = .5;
        static constexpr float exploration_factor = 2;
        static constexpr int   terminal_decay = 2;
        static constexpr int   terminal_loss = 6;


    };

}