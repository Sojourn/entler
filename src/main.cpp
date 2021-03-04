#include <iostream>

#include "memory/allocator.h"
#include "behavior_tree/behavior_tree.h"
#include "behavior_tree/nodes/sequence_node.h"
#include "entity/entity_component_database.h"

using namespace entler;

namespace Components {
    enum : uint32_t {
        transform,
        body,
        sprite,
        behavior,
    };
}

template<>
struct Component<Components::transform> {
    int x = 0;
    int y = 0;
};
using TransformComponent = Component<Components::transform>;

template<>
struct Component<Components::body> {
};
using BodyComponent = Component<Components::body>;

template<>
struct Component<Components::sprite> {
};
using SpriteComponent = Component<Components::sprite>;

template<>
struct Component<Components::behavior> {
};
using BehaviorComponent = Component<Components::behavior>;

int main(int argc, char** argv) {
    entler::EntityComponentDatabase<Components::transform, Components::sprite, Components::behavior> edb;
    edb.add_entity(
        TransformComponent {
            .x = 1,
            .y = 13,
        },
        SpriteComponent {
        },
        BehaviorComponent {
        }
    );

    auto result = edb.query<Components::transform, Components::behavior>();
    while (auto record = result.next()) {
        auto&& [transform, behavior] = record->get_components();
        std::cout << transform.x << ", " << transform.y << std::endl;
        record->remove();
    }
    while (auto record = result.next()) {
        auto&& [transform, behavior] = record->get_components();
        std::cout << transform.x << ", " << transform.y << std::endl;
        record->remove();
    }

    return 0;
}
