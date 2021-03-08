#pragma once

#include "entity/entity_schema.h"
#include "util/math.h"

namespace entler {

    enum class ComponentType {
        transform,
        body,
        display,
        energy,
    };

    template<>
    class Component<ComponentType, ComponentType::transform> {
        I32Vec3 position;
        I32Vec3 direction; // normal
    };

    template<>
    class Component<ComponentType, ComponentType::body> {
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
