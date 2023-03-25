#include "network_manager.h"

#include "../blowfish/defs.h"

#include "policy_index.h"

bool NetworkManager::LoadNetwork(std::string path) {
    this->network_path_ = path;
    model_.LoadFromCheckpoint(path); 
    print(path);   
}

cppflow::tensor NetworkManager::Preprocess(Board board, std::vector<std::string> history) {
    cppflow::tensor ret =  parser_.BoardToTensor(board, history);
    return cppflow::expand_dims(ret, 0);
}

void NetworkManager::SetNodeProbabilities(NodeGraph* nodemodel, const NetworkOutput& output) {
    std::vector<NodeGraph*> edges = nodemodel->GetEdges();
    if(edges.size() == 0) return;

    for(auto & edge : edges) {
        int neuralindex = edge->GetPolicyIndex();
        print(neuralindex);
        if(neuralindex != -1) {
            edge->SetP(output.probs_[neuralindex]);
        }
    }

    return;
}

IOutput NetworkManager::Parse(NodeGraph* nodemodel) {
    m_assert(nodemodel->GetExpanded(), "evaluation node is not expanded, missing information");
    auto input = this->Preprocess(nodemodel->GetBoard(), nodemodel->GetHistory());
    auto res = model_(input);
    
    NetworkOutput network_output;
    network_output.probs_ = res[0].get_data<float>();
    network_output.value_ = res[1].get_data<float>()[0];

    SetNodeProbabilities(nodemodel, network_output);

    print(network_output.value_);
    nodemodel->SetQ(network_output.value_);
    nodemodel->SetFrozen();
}