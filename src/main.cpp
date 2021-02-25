#include <iostream>

#include "memory/allocator.h"
#include "behavior_tree/behavior_tree.h"
#include "behavior_tree/nodes/sequence_node.h"
#include "util/registry.h"

int main(int argc, char** argv) {
    behavior_tree::RootNode root;
    root.add_child<behavior_tree::SequenceNode>();

    return 0;
}
