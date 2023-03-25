
add_library(neuralnet
    v0model.h
    v0model.cpp
    v0input_parser.h
    v0input_parser.cpp
    history.h
    history.cpp
    frozen_nodegraph.h
    frozen_nodegraph.cpp
    nodegraph_cache.h
    nodegraph_cache.cpp    
    network_manager.h
    network_manager.cpp


)   


target_link_libraries(neuralnet PUBLIC blowfish_engine mcts_new)