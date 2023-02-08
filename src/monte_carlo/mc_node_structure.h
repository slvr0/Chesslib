#pragma once 

#include <algorithm>

#include "../blowfish/chessboard.h"

#include "mc_node.h"

//i want the tree structure to initiate the search by setting root and tagging the first branches. 
//this structure is shared between insertion threads
class NodeTreeStructure {
public:
    NodeTreeStructure(const Board& board) :
        root_node_(std::make_unique<Node> (board)) {

    }

    ~NodeTreeStructure() {
        for(auto & br : root_node_->branches_)
            DeleteNode(br);
    }

    Node* Reset() const {
        return root_node_.get();
    }

    //checks if Node is available, else returns pointer to root
    //the thread needs to aquire a lock guard mutex when doing this.
    FORCEINL bool RequestNodeCheckIn(Node* node) {
        if(node->Available()) {            
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

    FORCEINL size_t GetTreeSize() const {
        return root_node_->GetEntries();
    }

private:
    size_t tree_size_ = 0;
    std::unique_ptr<Node> root_node_ = nullptr;
};