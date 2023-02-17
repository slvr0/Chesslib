/* #pragma once

#include <iostream>
#include <memory>
#include <vector>


#include "../blowfish/chessboard.h"
#include "../blowfish/chessboard_generator.h"



//MCTSNodeModel(Board, Meta, parent = NULL)
//MCTSNodeModel == (MCTSNodeModel other) return this.board.z = other.board.z
//MCTSNodeModel::iterator<const bool> defines forward iterating ?


class NodeSimple {
public:
    NodeSimple(const int &depth, const int &id, const Board& board, NodeSimple* parent = nullptr);

    void InsertNode(NodeSimple* node);
    void InsertNode(const int &depth, const int & id, const Board& board, NodeSimple* parent = nullptr);

    void UpdateResult(const SimResult& result);
   
    std::string Debug() const;
    std::string DebugTree() const;
    std::string DebugTreeSimple() const;

    float GetPolicyNonExpl() const;
    float GetPolicy() const;
    int GetVisits() const;

    void Display(const int& indent, const bool& display_simple = false) const;

    int GetId() const;
    std::vector<NodeSimple*> GetEdges() const;
    int GetW() const;
    bool IsLeaf() const;

    NodeSimple* GetBestPolicy() const;


private:
    NodeSimple*                 parent_ = nullptr;
    Board                       board_;
    int                         Q_ = 0;
    int                         N_ = 0;
    std::vector<NodeSimple*>    edges_;
    int                         id_ = 0;
    int                         depth_ = 0;

    bool                        is_white_ = true;
};

class NodeExpander{
public:
    NodeExpander() {
        for(int i = 0 ; i < 30; ++i) {
            levelentries_.push_back(0);
        }
    }

    void PrintTreeStructure() const;

    NodeSimple* CreateEdgeNode(NodeSimple* from, const int &depth, const Board& board);
    void IncrementRoot();

private:
    int ids_ = 0;
    std::vector<int> levelentries_;
};


class NodeTree {
public:
    NodeTree(const Board& board); 

    std::string DebugTree() const;

    
    NodeSimple* Reset() const;

    NodeSimple* CreateNode(NodeSimple* from, const int& depth,  const Board& board);
    inline void PrintTreeStructure() const {
        nexp_.PrintTreeStructure();
    }
    inline void PrintTree(const bool& simple = false) const {
        root_->Display(0, simple);
    }

private:
    std::unique_ptr<NodeSimple> root_;
    NodeExpander nexp_;
  
};

class MCTSNodeModelWorker {
    friend class NodeTree;
public:
    MCTSNodeModelWorker()
    SimResult SendForRolloutEval(NodeSimple* node);
    bool SendForNeuralEval(NodeSimple* node);

};

class NodeSelect {
public:
    
    NodeSimple* GetId(NodeSimple* root, const int& sid);
};

class NodeRollout {
public:
    static void         Backpropagate(NodeSimple* node, const SimResult& outcome);
    static SimResult    SimulateRandom(NodeSimple* node);
};

class NodeTreeToolbox {
public :
    NodeTreeToolbox()  {
        srand((unsigned)time(NULL));
    }

    void CreateTreeFormationSimple(std::string position);

private:
    std::unique_ptr<NodeTree> tree_ = nullptr;
    NodeExpander expander_;
};

 */




