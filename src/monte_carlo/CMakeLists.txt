
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
    
    mc_mainthread.h
    mc_mainthread.cpp
    mc_expansion.h
    mc_expansion.cpp
    mc_simulator.h
    mc_simulator.cpp
    mc_backprop.h
    mc_backprop.cpp

    mc_wmgenrollout.h
    mc_wmgenrollout.cpp
    mc_bmgenrollout.h
    mc_bmgenrollout.cpp
)   

target_link_libraries(mcts PUBLIC blowfish_engine)