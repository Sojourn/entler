#pragma once

#include <bitset>
#include <optional>
#include <tuple>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cassert>
#include "util/intrusive_list.h"
#include "entity_schema.h"

namespace entler {

    using EntityId = int64_t;

    template<typename Schema>
    class EntityDatabase;

    template<typename Schema>
    class Entity {
        template<typename> friend class EntityDatabase;
        template<typename> friend class EntityHandle;
        using Database = EntityDatabase<Schema>;

    public:
        using ComponentType = typename Schema::ComponentType;

        template<ComponentType component_type>
        using Component = entler::Component<ComponentType, component_type>;

    public:
        EntityId get_id() const;

        template<ComponentType component_type>
        bool has_component() const;
        bool has_component(ComponentType component_type) const;
        bool has_components(typename Schema::ComponentMask component_mask) const;

        template<ComponentType component_type>
        Component<component_type>& get_component();

        template<ComponentType component_type>
        const Component<component_type>& get_component() const;

    private:
        Entity(EntityDatabase<Schema>& database, size_t entity_index);

        Database& database_;
        size_t    entity_index_;
    };

    template<typename Schema>
    class EntityHandle {
        template<typename> friend class EntityDatabase;

    public:
        EntityHandle() = default;
        EntityHandle(Entity<Schema>& entity);

        explicit operator bool() const {
            return handle_list_node_.is_linked();
        }

        Entity<Schema> get();
        Entity<Schema> operator*();

        void reset();

    private:
        void update_entity_index(size_t entity_index);

        EntityDatabase<Schema>* entity_database_   = nullptr;
        size_t                  entity_index_      = 0;
        IntrusiveListNode       handle_list_node_;
    };

    template<typename Schema>
    class EntityObserver {
    public:
        EntityObserver(EntityDatabase<Schema>& entity_database);
        EntityObserver(EntityObserver&&) = delete;
        EntityObserver(const EntityObserver&) = delete;
        EntityObserver& operator=(EntityObserver&&) = delete;
        EntityObserver& operator=(const EntityObserver&) = delete;

        virtual ~EntityObserver();

        virtual void entity_added(Entity<Schema> entity) {}
        virtual void entity_removed(Entity<Schema> entity) {}

    private:
        EntityDatabase<Schema>& entity_database_;
    };

    template<typename Schema>
    class EntityDatabase {
        template<typename> friend class Entity;
        template<typename> friend class EntityHandle;
        template<typename> friend class EntityObserver;

    public:
        using ComponentType = typename Schema::ComponentType;
        using ComponentMask = typename Schema::ComponentMask;

        template<ComponentType component_type>
        using Component = entler::Component<ComponentType, component_type>;

    public:
        EntityDatabase();

        template<ComponentType... component_types>
        Entity<Schema> add_entity(Component<component_types>... components);

        void remove_entity(Entity<Schema> entity);

        // visits all entities
        template<typename Visitor>
        void for_each_entity(Visitor&& visitor);

        // visits entities that have all of the components in component_mask
        template<typename Visitor>
        void for_each_entity(std::initializer_list<ComponentType> component_types, Visitor&& visitor);

        void vacuum() {
            // TODO
        }

    private:
        using ComponentTables = typename Schema::ComponentTables;

        using EntityHandleList = IntrusiveList<EntityHandle<Schema>, &EntityHandle<Schema>::handle_list_node_>;

        // TODO: rename this to something else
        struct EntityRecord {
            EntityId         entity_id;
            ComponentMask    component_mask;
            size_t           component_indexes[Schema::component_type_count()];
            EntityHandleList handles;

            EntityRecord(EntityId entity_id)
                : entity_id(entity_id)
            {
                memset(component_indexes, 0, sizeof(component_indexes));
            }
        };

        void add_entity_observer(EntityObserver<Schema>& observer) {
            entity_observers_.push_back(&observer);
        }

        void remove_entity_observer(EntityObserver<Schema>& observer) {
            if (auto it = std::find(entity_observers_.begin(), entity_observers_.end(), &observer); it != entity_observers_.end()) {
                entity_observers_.erase(it);
            }
        }

        void notify_entity_added(Entity<Schema> entity) {
            for (EntityObserver<Schema>* observer: entity_observers_) {
                observer->entity_added(entity);
            }
        }

        void notify_entity_removed(Entity<Schema> entity) {
            for (EntityObserver<Schema>* observer: entity_observers_) {
                observer->entity_removed(entity);
            }
        }

        EntityRecord& get_entity_record(size_t entity_index) {
            assert(entity_index < entity_table_.size());
            return entity_table_[entity_index];
        }

        void update_entity_index(size_t entity_index) {
            for (auto& handle: get_entity_record(entity_index).handles) {
                handle.update_entity_index(entity_index);
            }
        }

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
        EntityId                             next_entity_id_;
        std::vector<EntityRecord>            entity_table_;
        ComponentTables                      component_tables_;
        std::vector<EntityObserver<Schema>*> entity_observers_;
    };

#include "entity_database_inline.h"

}
