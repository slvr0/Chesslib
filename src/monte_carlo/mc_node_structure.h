#pragma once 

#include <algorithm>

#include "../blowfish/chessboard.h"

#include "mc_node.h"
#include "mc_expansion.h"


//i want the tree structure to initiate the search by setting root and tagging the first branches. 
//this structure is shared between insertion threads

namespace MCTS {
class NodeTreeStructure {
public:
    NodeTreeStructure(const Board& board) :
        root_node_(std::make_unique<Node> (board)) {

    }

    ~NodeTreeStructure() {
        for(auto & br : root_node_->edges_)
            DeleteNode(br);
    }

    Node* Reset() const {
        return root_node_.get();
    }


    void CreateBranches() {
        m_assert(root_node_ && root_node_->IsLeaf(), "trying to branch root when its already expanded");

        MCMGenVerboseExpander expander;
        expander.Expand(root_node_.get());
    }

    void Evaluate();

    //checks if Node is available, else returns pointer to root
    //the thread needs to aquire a lock guard mutex when doing this.
    FORCEINL bool RequestNodeCheckIn(Node* node) {
        if(!node->IsLocked()) {            
            node->CheckIn();           
            return true;
        }
        else {
            return false;
        }  
    }

    //you return the node and retreive the root again
    FORCEINL void RequestNodeCheckOut(Node* node) {
        node->CheckOut();
    }

private:
    size_t tree_size_ = 0;
    std::unique_ptr<Node> root_node_ = nullptr;
};
}