#pragma once

#include <iostream>
#include <memory>
#include <list>
#include <cmath>
#include <algorithm>

#include "../blowfish/chessboard.h"

//contains the node structure the mcts tree consists of
class Node {
public : 
    const Board         board_;
    uint16_t            available_  = 0xFF;
    float               value_      = 0;
    size_t              visits_     = 0;
    std::vector<Node*>  branches_;
    Node*               parent_     = nullptr;
    

    FORCEINL Node(const Board & board, Node* parent = nullptr) : 
         board_(board), parent_(parent) {
    }

    ~Node() {

    }

    void Backpropagate(const float & res) {
        ++visits_;
        if(parent_) parent_->Backpropagate(res);
    }

    FORCEINL void CheckIn() {
        available_ = 0x00;
    }

    FORCEINL void CheckOut() {
        available_ = 0xFF;
    }

    FORCEINL bool Available() const {  
        return available_ ? true : false;
    }

    FORCEINL size_t GetEntries() const {
        static size_t   entries_ = 0;
        return entries_;
    }

    FORCEINL float UpperConfidenceBound() const {
        if(!Available()) return -10000.f;
        else if(visits_ == 0) return 10000.f;

        if(parent_ ) return (value_ / visits_) + (parent_->visits_ / visits_) - available_;
        else return -10000.f;
    }

    FORCEINL void Insert(Node* node) {
        static size_t       entries_ = 0;
        ++entries_;
        branches_.push_back(node);
    }

    FORCEINL void Insert(const Board& board) {
        static size_t       entries_ = 0;
        ++entries_;
        branches_.push_back(new Node(board, this));
    }

    FORCEINL bool IsLeaf() const {
        return branches_.empty();
    }

    bool operator==(Node* node) {
        return true; // in future compare board zobrist
    }

    bool operator<(Node* node) {
        return this->UpperConfidenceBound() < node->UpperConfidenceBound();
    }

/*     bool operator<(Node* n1, Node*n2) const {
        return n1->UpperConfidenceBound() < n2->UpperConfidenceBound();
    } */


    FORCEINL Node* GetUpperConfidenceBranch() const {
        return *std::max_element(branches_.begin(), branches_.end(), 
            [](Node* a, Node*b) { return a->UpperConfidenceBound() < b->UpperConfidenceBound();});

        std::vector<int> v {1,2,3};
        //std::max(branches_, [] (Node* n1, Node* n2) { return n1->UpperConfidenceBound() < n2->UpperConfidenceBound();});

/*         return std::max(branches_); */
        /* 
        print(branches_.size());
        auto nodeiter = 
        
        return *nodeiter; */
    }

    
};

//removes everything connected to the deleted node recursively
void DeleteNode(Node* node); 