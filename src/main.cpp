#include <iostream>

#include "behavior_tree/behavior_tree.h"
#include "behavior_tree/nodes/sequence_node.h"

int main(int argc, char** argv) {
    bt::RootNode root;
    root.add_child<bt::SequenceNode>();

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
