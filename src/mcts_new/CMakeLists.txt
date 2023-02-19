
add_library(mcts_new
    mcts_defines.h
    mcts_defines.cpp
    mcts_nodemodel.h
    mcts_nodemodel.cpp
    mcts_nodetree.h
    mcts_nodetree.cpp
    mcts_nodeinserter.h
    mcts_nodeinserter.cpp
    mcts_simulation_env.h
    mcts_simulation_env.cpp
    mcts_node_expander.h
    mcts_node_expander.cpp
    mcts_evaluationtool.h
    mcts_evaluationtool.cpp
    mcts_jobqueue.h
)   


target_link_libraries(mcts_new PUBLIC blowfish_engine)