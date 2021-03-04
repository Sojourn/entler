#pragma once

#include <optional>
#include <algorithm>
#include <cstdint>
#include "component.h"

namespace entler {

    struct Entity {
        uint32_t component_type_mask : 30;
        uint32_t tombstone : 1;
    };

}
