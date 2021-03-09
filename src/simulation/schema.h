#pragma once

#include "entity/entity_schema.h"
#include "components.h"

namespace entler {

    using Schema = EntitySchema<
        ComponentType,
        ComponentType::object_type,
        ComponentType::property_type,
        ComponentType::position,
        ComponentType::body,
        ComponentType::display,
        ComponentType::energy
    >;

}
