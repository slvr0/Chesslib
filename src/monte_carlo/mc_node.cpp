#include "mc_node.h"

void DeleteNode(Node* node) {
    for(auto & branch : node->branches_) {
        DeleteNode(branch);
    }

    delete(node);
}
