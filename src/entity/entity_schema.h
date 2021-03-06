#pragma once

#include <bitset>
#include <optional>
#include <tuple>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cassert>

namespace entler {

    template<typename ComponentType, ComponentType component_type>
    class Component;

    template<typename ComponentType_, ComponentType_... component_types>
    class EntitySchema {
    public:
        using ComponentType = ComponentType_;
        using ComponentMask = std::bitset<sizeof...(component_types)>;

        template<ComponentType component_type>
        using Component = entler::Component<ComponentType, component_type>;

        using ComponentTables = std::tuple<
            std::vector<
                Component<component_types>
            >...
        >;

    public:
        static constexpr size_t component_type_count() {
            return sizeof...(component_types);
        }

        static constexpr ComponentType get_component_type(size_t component_type_index) {
            ComponentType component_type_array[] = {
                    component_types...
            };

            return component_type_array[component_type_index];
        }

        static constexpr std::optional<size_t> find_component_type(ComponentType component_type) {
            for (size_t component_type_index = 0; component_type_index < component_type_count(); ++component_type_index) {
                if (component_type == get_component_type(component_type_index)) {
                    return component_type_index;
                }
            }

            return std::nullopt;
        }
    };

}
