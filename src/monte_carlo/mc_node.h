#pragma once

#include <iostream>
#include <memory>
#include <list>
#include <cmath>
#include <algorithm>
#include <iomanip>

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
    int                     wl_         = 0;
    int                     d_          = 0;
    int                     N_          = 0;

    TerminalState           terminal_   = TerminalState::NonTerminal;

    Bounds                  bounds_;
    bool                    solid_ = false; 

    Node(const Board & board, Node* parent = nullptr);
    Node(const Board & board, std::string verbose, Node* parent = nullptr); 

    void        Insert(Node* node);
    void        Insert(const Board& board);
    void        Insert(const Board& board, const std::string & verbose);
    
    void        Backpropagate(GameResult result); 
    void        PropagateTerminal(const int& update_wl, const int&update_d, const int & distance);

    void        CheckIn();
    void        CheckOut();
    bool        IsLocked() const;
    bool        IsLeaf() const;
    std::string Debug() const;

    void        UpdateQFactor();
    void        UpdateNodePolicy(const bool& add_dirichlet_noise = false);

    Node*       GetBestQNode(QualityFactorMethods qfmethod) const;
    Node*       GetBestPNode() const;

    void        SetTerminal(TerminalState terminal);
    void        SetBounds(Bounds bounds);
    void        SetSolid();

    Bounds      GetBounds() const;
    TerminalState GetTerminal() const;
    bool        IsSolid() const;

    double GetScore();

    void PrintChildPolicies() {
            std::cout << std::fixed;
            std::cout << std::setprecision(2) << verbose_ << "(HEAD):" << this->GetScore() << " |N:"<<N_<<std::endl;      
        for(auto & n : edges_ ) {
            double pol = n->GetScore();
            std::cout << std::fixed;
            std::cout << std::setprecision(2) << verbose_ << ":" << pol << " |N:"<< n->N_<< std::endl;
    
        }

    }

};
}

//removes everything connected to the deleted node recursively
void DeleteNode(MCTS::Node* node); 