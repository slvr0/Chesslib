#ifndef MCTS_NODEMODEL_H
#define MCTS_NODEMODEL_H

#include <iostream>

#include "../blowfish/chessboard.h"

#include "mcts_defines.h"

#define NodeDebug 1;

class NodeInfo {
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

class MCTSNodeModel {
public:
    MCTSNodeModel(const Board& board, const NodeInfo& ninfo, MCTSNodeModel* parent = nullptr);
    
    void InsertNode(MCTSNodeModel* node);
    void InsertNode(const Board& board, NodeInfo ninfo, MCTSNodeModel* parent = nullptr);
    void BackpropagateScoreUpdate(const SimulationResult& result);
   
    //getters
    std::vector<MCTSNodeModel*> GetEdges()      const;
    MCTSNodeModel* GetBestPolicy()              const;
    float       GetPolicyNonExpl()              const;
    float       GetPolicy()                     const;
    NodeInfo    GetInfo()                       const;
    int         GetWinLooseRatio()              const;
    int         GetDraws()                      const;
    int         GetVisits()                     const;  
    bool        IsLeaf()                        const;  
    bool        IsWhite()                       const;
    Board       GetBoard()                      const;
    int         GetW()                          const;
    int         GetD()                          const;
    MCTSNodeModel* GetParent()                  const;
    bool        RemoveParent();
    bool        Detach(MCTSNodeModel* node);

    void        DebugDisplay(const int& indent, const bool& display_simple = false) const;
    std::string NodeDisplaySimple() const;
    std::string NodeDisplayFull()   const;

private:
    const Board                 board_;
    const NodeInfo              info_;
    std::vector<MCTSNodeModel*> edges_;
    MCTSNodeModel*              parent_ = nullptr;
 
    //policy metrics
    int         WL_     = 0;
    uint64_t    D_      = 0;
    uint64_t    N_      = 0;
};

#endif