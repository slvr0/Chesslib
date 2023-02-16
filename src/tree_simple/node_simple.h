#pragma once

#include <iostream>
#include <memory>
#include <vector>

enum class SimResult {
    UNDECISIVE = 0, 
    WHITEWIN = 1,
    BLACKWIN = 2,
    DRAW = 3
};

class NodeSimple {
public:
    NodeSimple(const int &depth, const int &id, const int &value = 0, NodeSimple* parent = nullptr);

    void InsertNode(NodeSimple* node);
    void InsertNode(const int &depth, const int & id, const int &value = 0, NodeSimple* parent = nullptr);

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

    NodeSimple* CreateEdgeNode(NodeSimple* from, const int &depth, const int&value = 0);
    void IncrementRoot();

private:
    int ids_ = 0;
    std::vector<int> levelentries_;
};


class NodeTree {
public:
    NodeTree(const int & rootval); 

    std::string DebugTree() const;

    
    NodeSimple* Reset() const;

    NodeSimple* CreateNode(NodeSimple* from, const int& depth,  const int& value = 0);
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

    void CreateTreeFormationSimple();

private:
    std::unique_ptr<NodeTree> tree_ = nullptr;
    NodeExpander expander_;
};






