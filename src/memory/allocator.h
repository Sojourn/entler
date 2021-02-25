#pragma once

#include "behavior_tree/allocator.h"

// TODO: move allocator into a proper library
namespace entler {
    using Allocator = behavior_tree::Allocator;
    using GlobalAllocator = behavior_tree::GlobalAllocator;
    using ArenaAllocator = behavior_tree::ArenaAllocator;
}
