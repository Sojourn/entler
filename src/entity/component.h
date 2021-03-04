#pragma once

#include <cstdint>

namespace entler {

    // TODO: rename to ComponentTypeId
    using ComponentTypeId     = uint32_t;
    using ComponentTypeMask = uint32_t;

    template<ComponentTypeId component_type>
    struct Component;

    template<ComponentTypeId... types>
    inline constexpr ComponentTypeId get_component_type(size_t index) {
        ComponentTypeId type_array[] = {
            types...
        };

        return type_array[index];
    }

    template<ComponentTypeId... types>
    inline constexpr int find_component_type(ComponentTypeId type) {
        for (int i = 0; i < sizeof...(types); ++i) {
            if (type == get_component_type<types...>(static_cast<size_t>(i))) {
                return i;
            }
        }

        return -1;
    }

    template<ComponentTypeId... types>
    inline constexpr ComponentTypeMask make_component_type_mask() {
        ComponentTypeMask mask = 0;
        for (size_t i = 0; i < sizeof...(types); ++i) {
            ComponentTypeMask mask_part = (1u << get_component_type<types...>(i));
            mask |= mask_part;
        }

        return mask;
    }

    template<ComponentTypeId... types>
    inline constexpr bool unique_component_types() {
        ComponentTypeMask mask = 0;
        for (size_t i = 0; i < sizeof...(types); ++i) {
            ComponentTypeMask mask_part = (1u << get_component_type<types...>(i));
            if (mask & mask_part) {
                return false; // we've already seen this component type -> not unique
            }

            mask |= mask_part;
        }

        return true;
    }

}
