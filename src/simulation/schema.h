#pragma once

#include "entity/entity_schema.h"
#include "components.h"

namespace entler {

    using Schema = EntitySchema<
        ComponentType,
        ComponentType::transform,
        ComponentType::body,
        ComponentType::display,
        ComponentType::energy
    >;

}
