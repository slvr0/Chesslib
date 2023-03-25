#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <iostream>
#include <map>
#include <vector>


#include "../mcts_new/mcts_nodegraph.h"

#include "v0input_parser.h"
#include "v0model.h"

/*

loads network (and reloads), 
stores final nodemodel, writes results into chunked files, 
reprocesses input, and returns cleaned data output.

*/

struct IOutput {

};

struct MappedNetworkOutput : public IOutput{
    std::map<int, float> probs_;
    float value_;
};

struct NetworkOutput : public IOutput{
    std::vector<float> probs_;
    float value_;
};

class NetworkManager {
public:

NetworkManager() {   
   
}

bool    LoadNetwork(std::string path);
IOutput Parse(NodeGraph* nodemodel);

private:
    cppflow::tensor Preprocess(Board board, std::vector<std::string> history);
    void            SetNodeProbabilities(NodeGraph* nodemodel, const NetworkOutput& output);

private:
    std::string     network_path_;
    V0NetworkModel  model_;
    V0InputParser   parser_; 
};

#endif