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
    uint16_t            available_  = 0x0;
    float               value_      = 0;
    int                 visits_     = 0;
    std::vector<Node*>  branches_;
    Node*               parent_     = nullptr;
    

    FORCEINL Node(const Board & board, Node* parent = nullptr) : 
         board_(board), parent_(parent) {
    }

    ~Node() {

    }

    //draws are shared, wins are not
    void Backpropagate(const float & res, const bool& white) {
        ++visits_;

        if(res == .5f) value_ += res; 
        
        if(board_.white_acts_ == white) {
            value_ += res;
        } 

        value_ += res;

        if(parent_) parent_->Backpropagate(res, white);
    }

    void Backpropagate(const float & res) {
        ++visits_;

        value_ += res;   

        if(parent_) parent_->Backpropagate(res, board_.white_acts_);
    }

    FORCEINL void CheckIn() {
        available_ = 0xFF;
    }

    FORCEINL void CheckOut() {
        available_ = 0x0;
    }

    FORCEINL bool Available() const {  
        return available_ ? false : true;
    }

    FORCEINL size_t GetEntries() const {
        static size_t   entries_ = 0;
        return entries_;
    }

    FORCEINL float UpperConfidenceBound() const {
        if(!Available()) return -10000.f;
        else if(visits_ == 0) return 10000.f;

        if(parent_ ) return ((float)(value_ / visits_) + (2 * std::sqrt(std::log1p(parent_->visits_) / visits_))) - available_;
        else return -50000.f;
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

    FORCEINL Node* GetUpperConfidenceBranch() const {
        return *std::max_element(branches_.begin(), branches_.end(), 
            [](Node* a, Node*b) { return a->UpperConfidenceBound() < b->UpperConfidenceBound();});     

    }
};

//removes everything connected to the deleted node recursively
void DeleteNode(Node* node); 