#pragma once

#include <vector>
#include <tuple>
#include <optional>
#include <iterator>
#include <compare>
#include <cstdint>
#include <cstring>
#include "entity.h"
#include "component.h"

namespace entler {

    template<ComponentTypeId... component_type_ids_>
    class EntityComponentDatabase {
    public:
        EntityComponentDatabase() = default;

        template<ComponentTypeId... component_type_ids>
        void add_entity(Component<component_type_ids>... components) {
            static_assert(sizeof...(component_type_ids) > 0, "An entity must have at least one component");
            static_assert(unique_component_types<component_type_ids...>(), "Component types must be unique");
            static constexpr ComponentTypeMask component_type_mask = make_component_type_mask<component_type_ids...>();

            Entity entity;
            entity.component_type_mask = component_type_mask;
            entity.tombstone = 0;
            entity_table_.push_back(entity);

            add_components(std::move(components)...);
        }

    public:
        template<ComponentTypeId... component_type_ids>
        class QueryResult {
        public:
            class Record {
                template<ComponentTypeId...> friend class QueryResult;
            public:
                Record(EntityComponentDatabase& database)
                    : database_(database)
                    , removed_(false)
                    , entity_count_(static_cast<int32_t>(database.entity_table_.size()))
                    , entity_offset_(-1)
                {
                    for (int32_t& component_offset: component_offsets_) {
                        component_offset = -1;
                    }
                }

                template<ComponentTypeId component_type_id>
                Component<component_type_id>& get_component() {
                    assert(!removed_);
                    assert(0 <= entity_offset_ && entity_offset_ < entity_count_);

                    static constexpr int component_type_index = find_component_type<component_type_ids...>(component_type_id);
                    static_assert(component_type_index >= 0, "Unknown component");

                    return database_.get_component<component_type_id>(component_offsets_[component_type_index]);
                }

                std::tuple<Component<component_type_ids>&...> get_components() {
                    assert(!removed_);
                    assert(0 <= entity_offset_ && entity_offset_ < entity_count_);

                    return make_tuple<component_type_ids...>();
                }

                void remove() {
                    assert(!removed_);
                    assert(0 <= entity_offset_ && entity_offset_ < entity_count_);

                    Entity& entity = database_.entity_table_[entity_offset_];
                    entity.tombstone = 1;
                }

            private:
                template<ComponentTypeId tup_component_type_id, ComponentTypeId... tup_component_type_ids>
                std::tuple<Component<tup_component_type_id>&, Component<tup_component_type_ids>&...> make_tuple() {
                    auto& component = get_component<tup_component_type_id>();
                    auto& component_tuple = std::tie(component);

                    if constexpr (sizeof...(tup_component_type_ids)) {
                        return std::tuple_cat(component_tuple, make_tuple<tup_component_type_ids...>());
                    }
                    else {
                        return component_tuple;
                    }
                }

            private:
                bool advance() {
                    while (++entity_offset_ < entity_count_) {
                        const Entity& entity = database_.entity_table_[entity_offset_];

                        // check if this entity contains none of the component types
                        if (!(entity.component_type_mask & component_type_mask_)) {
                            continue;
                        }

                        for (size_t component_type_index = 0; component_type_index < sizeof...(component_type_ids); ++component_type_index) {
                            ComponentTypeId component_type_id = get_component_type<component_type_ids...>(component_type_index);
                            if (entity.component_type_mask & (1u << component_type_id)) {
                                component_offsets_[component_type_index] += 1;
                            }
                        }

                        // check if this entity is active and contains all of the component types
                        if (!entity.tombstone && ((entity.component_type_mask & component_type_mask_) == component_type_mask_)) {
                            return true;
                        }
                    }

                    entity_offset_ = entity_count_;
                    return false;
                }

            private:
                static constexpr ComponentTypeMask component_type_mask_ = make_component_type_mask<component_type_ids...>();

                EntityComponentDatabase& database_;
                bool                     removed_;
                int32_t                  entity_count_;
                int32_t                  entity_offset_;
                int32_t                  component_offsets_[sizeof...(component_type_ids)];
            };

            QueryResult(EntityComponentDatabase& database)
                : record_(database)
            {
            }

            std::optional<Record> next() {
                if (record_.advance()) {
                    return record_;
                }

                return std::nullopt;
            }

        private:
            Record record_;
        };

        template<ComponentTypeId... component_type_ids>
        QueryResult<component_type_ids...> query() {
            static_assert(sizeof...(component_type_ids) > 0, "An entity must have at least one component");
            static_assert(unique_component_types<component_type_ids...>(), "Component types must be unique");

            return QueryResult<component_type_ids...>(*this);
        }

    private:
        template<ComponentTypeId component_type_id, ComponentTypeId... component_type_ids>
        void add_components(Component<component_type_id> component, Component<component_type_ids>... components) {
            static constexpr int component_table_index = find_component_type<component_type_ids_...>(component_type_id);
            static_assert(component_table_index >= 0, "Unknown component type");

            // add the component
            auto& component_table = std::get<component_table_index>(component_tables_);
            component_table.push_back(std::move(component));

            // add remaining components (if any)
            if constexpr (sizeof...(component_type_ids) > 0) {
                add_components(std::move(components)...);
            }
        }

        template<ComponentTypeId component_type_id>
        Component<component_type_id>& get_component(size_t component_offset) {
            static constexpr int component_table_index = find_component_type<component_type_ids_...>(component_type_id);
            static_assert(component_table_index >= 0, "Unknown component type");

            auto& component_table = std::get<component_table_index>(component_tables_);
            assert(component_offset < component_table.size());
            return component_table[component_offset];
        }

        template<ComponentTypeId component_type_id>
        const Component<component_type_id>& get_component(size_t component_offset) const {
            static constexpr int component_table_index = find_component_type<component_type_ids_...>(component_type_id);
            static_assert(component_table_index >= 0, "Unknown component type");

            auto& component_table = std::get<component_table_index>(component_tables_);
            assert(component_offset < component_table.size());
            return component_table[component_offset];
        }

    private:
        using EntityTable = std::vector<Entity>;
        using ComponentTables = std::tuple<
            std::vector<
                Component<component_type_ids_>
            >...
        >;

        EntityTable     entity_table_;
        ComponentTables component_tables_;
    };

}
