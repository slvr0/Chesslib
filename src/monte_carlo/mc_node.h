#pragma once

#include <iostream>
#include <memory>
#include <list>
#include <cmath>
#include <algorithm>

#include "../blowfish/chessboard.h"
#include "mc_config.h"
#include "mc_defs.h"
//contains the node structure the mcts tree consists of


//comparing Node structure with Leela
//Leela has a Node parent which links to Edges (branches), which can be dangling (not set)  solid (evaluated) or non solid (still being evaluated)
//They can detach nodes and move to other threads, variables inside the Node is specifically guarded by global mutexes to never be set while read
//They have a Bound of evaluation of the Node, upper / lower, which is the worst/best state possible ie startpos is set to {blackwin, whitewin} while 
//a position where black can never checkmate white would set {draw, whitewin}
//They can make nodes Terminal with different specific Terminal states (2fold, stalemate, checkmated)
//The UCB is called policy and is updated on demand, not calculated via every comparison. ie propagation updates it
//the backpropagation stores wins / losses and draws separate
//They have advanced caching of parts of node tree

//We should borrow logic : Policy update on demand, Make nodes terminal, separate storage of w/l and d, to start with
//also, they have a templated is_const bool Iterator for going through edges that is friended in Node, not exactly sure what this accomplishes but there is probably a smart reason



//make a debug string

namespace MCTS {
class Node {
public : 
    const Board             board_;
    Node*                   parent_     = nullptr;
    std::vector<Node*>      edges_;

    uint16_t                checkin_    = 0x0;
    std::string             verbose_    = ""; //in future , this will be removed and we shall map branches with each tag 

    //metrics
    float                   quality_    = 0.f; //Quality factor, could just get this as its fetched in the end?
    float                   policy_     = 0.f;
    uint32_t                wl_         = 0;
    uint32_t                d_          = 0;
    uint32_t                N_          = 0;

    TerminalState           terminal_   = TerminalState::NonTerminal;

    Node(const Board & board, Node* parent = nullptr);
    Node(const Board & board, std::string verbose, Node* parent = nullptr); 

    void        Insert(Node* node);
    void        Insert(const Board& board);
    void        Insert(const Board& board, const std::string & verbose);
    
    void        Backpropagate(GameResult result); 

    void        CheckIn();
    void        CheckOut();
    bool        IsLocked() const;
    bool        IsLeaf() const;
    std::string Debug() const;

    void        UpdateQFactor();
    void        UpdateNodePolicy(const bool& add_dirichlet_noise = false);

    Node*       GetBestQNode(QualityFactorMethods qfmethod) const;
    Node*       GetBestPNode() const;

    void        MakeTerminal(TerminalState terminal);
};
}

//removes everything connected to the deleted node recursively
void DeleteNode(MCTS::Node* node); 