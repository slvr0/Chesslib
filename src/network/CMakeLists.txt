
add_library(neuralnet
    v0model.h
    v0model.cpp
    history.h
    history.cpp


)   


target_link_libraries(neuralnet PUBLIC blowfish_engine mcts_new)