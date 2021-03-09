#pragma once

#include "entity/entity_schema.h"
#include "util/math.h"

namespace entler {

    enum class ObjectType {
        robot,
        rock,
        ball,
        block,
    };

    enum class PropertyType {
        hole,
        lava,
        mud,
    };

    enum class ComponentType {
        object_type,
        property_type,
        position,
        rotation,
        body,
        display,
        energy,
    };

    template<>
    class Component<ComponentType, ComponentType::object_type> {
    public:
        ObjectType type;
    };

    template<>
    class Component<ComponentType, ComponentType::property_type> {
    public:
        PropertyType type;
    };

    template<>
    class Component<ComponentType, ComponentType::position> {
    public:
        I32Vec3 value;
    };

    template<>
    class Component<ComponentType, ComponentType::rotation> {
    public:
        I32Vec3 value;
    };

    template<>
    class Component<ComponentType, ComponentType::body> {
    public:
        I32Vec3 velocity;
        I32Vec3 momentum;
    };

    template<>
    class Component<ComponentType, ComponentType::display> {
    public:
        char name[2] = { 0, 0 };
        int  color = 0;
    };

    template<>
    class Component<ComponentType, ComponentType::energy> {
        int value = 0;
        int capacity = 0;
        int recharge_rate = 0;
    };

}
