#include <iostream>

#include "memory/allocator.h"
#include "behavior_tree/behavior_tree.h"
#include "behavior_tree/nodes/sequence_node.h"
#include "entity/entity_database.h"

using namespace entler;

enum class ComponentType {
    transform,
    sprite,
    behavior,
};

template<>
class Component<ComponentType, ComponentType::transform> {
public:
    int x;
    int y;
    int z;
};

template<>
class Component<ComponentType, ComponentType::sprite> {
public:
    int sprite_id;
};

template<>
class Component<ComponentType, ComponentType::behavior> {
public:
};

using TransformComponent = Component<ComponentType, ComponentType::transform>;
using SpriteComponent = Component<ComponentType, ComponentType::sprite>;
using BehaviorComponent = Component<ComponentType, ComponentType::behavior>;

int main(int argc, char** argv) {
    EntityDatabase<EntitySchema<ComponentType, ComponentType::transform, ComponentType::sprite, ComponentType::behavior>> db;

    auto entity = db.add_entity(
        TransformComponent {
            1, 2, 3
        },
        SpriteComponent {
            1337
        }
    );

    TransformComponent& transform = entity.get_component<ComponentType::transform>();
    std::cout << transform.x << std::endl;
    std::cout << transform.y << std::endl;
    std::cout << transform.z << std::endl;

    db.for_each_entity({ComponentType::behavior}, [&](auto& entity) {
        std::cout << entity.get_id() << std::endl;
    });

    return 0;
}
