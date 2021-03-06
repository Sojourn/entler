#pragma once

#include <bitset>
#include <optional>
#include <tuple>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cassert>
#include "entity_schema.h"

namespace entler {

    using EntityId = int64_t;

    template<typename Schema>
    class Entity;

    template<typename Schema>
    class EntityDatabase;

    template<typename Schema>
    class Entity {
        template<typename> friend class EntityDatabase;
        using Database = EntityDatabase<Schema>;

    public:
        using ComponentType = typename Schema::ComponentType;

        template<ComponentType component_type>
        using Component = entler::Component<ComponentType, component_type>;

    public:
        EntityId get_id() const;

        template<ComponentType component_type>
        Component<component_type>& get_component();

        template<ComponentType component_type>
        const Component<component_type>& get_component() const;

    private:
        Entity(Database& database, size_t entity_index)
            : database_(database)
            , entity_index_(entity_index)
        {
        }

        Database& database_;
        size_t    entity_index_;
    };

    template<typename Schema>
    class EntityDatabase {
        template<typename> friend class Entity;

    public:
        using ComponentType = typename Schema::ComponentType;
        using ComponentMask = typename Schema::ComponentMask;

        template<ComponentType component_type>
        using Component = entler::Component<ComponentType, component_type>;
        using Entity = entler::Entity<Schema>;

    public:
        EntityDatabase()
            : next_entity_id_(0)
        {
        }

        template<ComponentType... component_types>
        Entity add_entity(Component<component_types>... components) {
            EntityRecord record(next_entity_id_++);
            add_components(record, std::move(components)...);

            size_t entity_index = entity_table_.size();
            entity_table_.push_back(record);

            return Entity(*this, entity_index);
        }

        void remove_entity(Entity& entity) {
            EntityRecord& record = entity_table_[entity.entity_index_];
            record.entity_id = -1;

            size_t component_type_index = 0;
            for_each_component_table([&](auto&& component_table) {
                if (record.component_mask.test(component_type_id)) {
                    size_t component_index = record.component_indexes[component_type_index];

                    // move the component onto the stack so it can free resources
                    auto component = std::move(component_table[component_index]);
                    (void)component;
                }

                component_type_index += 1;
            });
        }

        // visits all entities
        template<typename Visitor>
        void for_each_entity(Visitor&& visitor) {
            size_t entity_count = entity_table_.size();
            for (size_t entity_index = 0; entity_index < entity_count; ++entity_index) {
                EntityRecord& record = entity_table_[entity_index];
                if (record.entity_id < 0) {
                    continue;
                }

                visitor(Entity(*this, entity_index));
            }
        }

        // visits entities that have all of the components in component_mask
        template<typename Visitor>
        void for_each_entity(std::initializer_list<ComponentType> component_types, Visitor&& visitor) {
            ComponentMask component_mask;
            for (ComponentType component_type: component_types) {
                std::optional<size_t> component_type_index = Schema::find_component_type(component_type);
                assert(component_type_index);
                component_mask.set(*component_type_index);
            }

            size_t entity_count = entity_table_.size();
            for (size_t entity_index = 0; entity_index < entity_count; ++entity_index) {
                EntityRecord& record = entity_table_[entity_index];
                if (record.entity_id < 0) {
                    continue;
                }
                if ((record.component_mask & component_mask) != component_mask) {
                    continue;
                }

                visitor(Entity(*this, entity_index));
            }
        }

        void vacuum();

    private:
        using ComponentTables = typename Schema::ComponentTables;

        // TODO: rename this to something else
        struct EntityRecord {
            EntityId      entity_id;
            ComponentMask component_mask;
            size_t        component_indexes[Schema::component_type_count()];

            EntityRecord(EntityId entity_id)
                : entity_id(entity_id)
            {
                memset(component_indexes, 0, sizeof(component_indexes));
            }
        };

        template<ComponentType component_type>
        void add_component(EntityRecord& record, Component<component_type> component) {
            static constexpr auto component_type_index = Schema::find_component_type(component_type);
            static_assert(component_type_index, "Unknown component type");

            auto& component_table = std::get<*component_type_index>(component_tables_);
            record.component_mask.set(*component_type_index);
            record.component_indexes[*component_type_index] = component_table.size();
            component_table.push_back(std::move(component));
        }

        template<ComponentType component_type, ComponentType... component_types>
        void add_components(EntityRecord& record, Component<component_type> component, Component<component_types>... components) {
            add_component(record, std::move(component));
            if constexpr (sizeof...(component_types)) {
                add_components(record, std::move(components)...);
            }
        }

        template<typename F, size_t component_type_index = 0>
        void for_each_component_table(F&& f) {
            if constexpr (component_type_index < Schema::component_type_count()) {
                f(std::get<component_type_index>(component_tables_));
                for_each_component_table<F, component_type_index + 1>(std::forward<F>(f));
            }
        }

    private:
        EntityId                  next_entity_id_;
        std::vector<EntityRecord> entity_table_;
        ComponentTables           component_tables_;
    };

    template<typename Schema>
    EntityId Entity<Schema>::get_id() const {
        return database_.entity_table_[entity_index_].entity_id;
    }

    template<typename Schema>
    template<typename Schema::ComponentType component_type>
    auto Entity<Schema>::get_component() -> Component<component_type>& {
        static constexpr auto component_type_index = Schema::find_component_type(component_type);
        static_assert(component_type_index, "Unknown component type");

        const Database::EntityRecord& record = database_.entity_table_[entity_index_];
        size_t component_index = record.component_indexes[*component_type_index];
        return std::get<*component_type_index>(database_.component_tables_)[component_index];
    }

    template<typename Schema>
    template<typename Schema::ComponentType component_type>
    auto Entity<Schema>::get_component() const -> const Component<component_type>& {
        static constexpr auto component_type_index = Schema::find_component_type(component_type);
        static_assert(component_type_index, "Unknown component type");

        const Database::EntityRecord& record = database_.entity_table_[entity_index_];
        size_t component_index = record.component_indexes[*component_type_index];
        return std::get<*component_type_index>(database_.component_tables_)[component_index];
    }

}
