#ifndef MCTS_NODEGRAPH_H
#define MCTS_NODEGRAPH_H

#include <iostream>

#include "../blowfish/chessboard.h"

#include "mcts_defines.h"

#define NodeDebug 1;

class NodeInfo {
    friend class NodeGraph;
public :
    NodeInfo(const int& depth, const int& id, std::string verbose = "");  

    int         GetDepth() const;
    int         GetId() const;
    std::string GetMove() const;

private:
    const int depth_;
    const int id_;
    std::string verbose_;
};

//1 Add history on insertion, take parent and append move (Done)
//2 Add zobrist key has hash key somewhere to prepare for cache implementation
//3. 


//Add dirichlet noise and temperature to PolicyEval
class NodeGraph {
public:
    NodeGraph(const Board& board, const NodeInfo& ninfo, NodeGraph* parent = nullptr);
    
    void InsertNode(NodeGraph* node);
    void InsertNode(const Board& board, NodeInfo ninfo, NodeGraph* parent = nullptr);
    void BackpropagateScoreUpdate(const SimulationResult& result);
   
    std::vector<NodeGraph*> GetEdges() const;
    NodeGraph*  GetBestPolicy() const;
    float       GetPolicyNonExpl() const;
    float       GetPolicy() const;
    NodeInfo    GetInfo() const;
 
    bool        IsLeaf() const;  
    bool        IsWhite() const;
    Board       GetBoard() const;

    void        SetP(float value);
    float       GetP() const;
    void        SetQ(float value);
    float       GetQ() const;

    int         GetW() const;
    int         GetD() const;
    int         GetL() const;
    int         GetVisits() const;

    NodeGraph*  GetParent() const;
    bool        RemoveParent();
    bool        Detach(NodeGraph* node);
    std::vector<std::string> GetHistory() const;
    void        SetExpanded(bool v);
    bool        GetExpanded() const;
    void        SetPolicyIndex();
    int         GetPolicyIndex() const;
    void        SetFrozen();
    bool        GetFrozen();

    void        SetHistory();
    void        SetHistory(std::vector<std::string> history);


    void        DebugDisplay(const int& indent, const bool& display_simple = false) const;
    std::string NodeDisplaySimple() const;
    std::string NodeDisplayFull()   const;

private:
    const Board                 board_;
    const NodeInfo              info_;
    std::vector<NodeGraph*>     edges_;
    NodeGraph*                  parent_ = nullptr;
    std::vector<std::string>    history_;
    bool                        expanded_ = false; //indicates the node has been expanded by a move search in tree (edges has been set, verbose and policy index are present)
    bool                        frozen_ = false; //indicates the node has been evaluated by network
    int                         policy_index_ = -1;
 
    //metrics
    float       P_      = 0.0f;
    float       Q_      = 0.0f;
    int         W_      = 0;
    int         D_      = 0;
    int         L_      = 0;
    int         N_      = 0;
};

#endif