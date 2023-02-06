
add_library(mcts
    mcts.h
    mcts.cpp
    
    mc_functions.h
    
    mc_node.h
    mc_node.cpp
    mc_node_structure.h
    mc_node_structure.cpp

    config.h

    mc_thread_factory.h
    mc_thread_factory.cpp
    
    mc_insert_thread.h
    mc_insert_thread.cpp
    mc_expansion.h
    mc_expansion.cpp
    mc_simulation_thread.h
    mc_simulation_thread.cpp
    mc_backprop.h
    mc_backprop.cpp
)