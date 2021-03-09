#pragma once

#include <memory>
#include <vector>
#include "entity/entity_database.h"
#include "schema.h"

namespace entler {

    class Scene : public EntityObserver<Schema> {
    public:
        Scene(EntityDatabase<Schema>& database, size_t width, size_t height)
            : EntityObserver<Schema>(database)
            , width_(width)
            , height_(height)
            , objects_(new EntityHandle<Schema>[width * height])
            , properties_(new std::vector<EntityHandle<Schema>>[width * height])
        {
        }

        void add_object(Entity<Schema> entity) {
            assert(entity.has_component<ComponentType::position>());
            assert(entity.has_component<ComponentType::object_type>());

            auto& position = entity.get_component<ComponentType::position>();
            auto offset = get_offset(position.value);

            assert(!objects_[offset]);
            objects_[offset] = EntityHandle<Schema>(entity);
        }

        void add_property(Entity<Schema> entity) {
            assert(entity.has_component<ComponentType::position>());
            assert(entity.has_component<ComponentType::object_type>());

            auto& position = entity.get_component<ComponentType::position>();
            auto offset = get_offset(position.value);

            properties_[offset].push_back(EntityHandle<Schema>(entity));
        }

        void move_object(Entity<Schema> entity, I32Vec3 new_position) {
            assert(entity.has_component<ComponentType::position>());
            assert(entity.has_component<ComponentType::object_type>());

            auto& position = entity.get_component<ComponentType::position>();
            auto old_offset = get_offset(position.value);
            auto new_offset = get_offset(new_position);

            assert(entity.has_component<ComponentType::object_type>());
            assert(objects_[old_offset]);
            assert(!objects_[new_offset]);

            objects_[new_offset] = std::move(objects_[old_offset]);
            position.value = new_position;
        }

        std::optional<Entity<Schema>> get_object(I32Vec3 position) {
            size_t offset = get_offset(position);
            auto& handle = objects_[offset];
            if (handle) {
                return *handle;
            }

            return std::nullopt;
        }

        template<typename Visitor>
        void for_each_property(I32Vec3 position, Visitor&& visitor) {
            size_t offset = get_offset(position);
            for (auto& handle: properties_[offset]) {
                if (handle) {
                    visitor(*handle);
                }
            }
        }

    private:
        size_t get_offset(I32Vec3 position) const {
            return position.x + (position.y * width_);
        }

    private:
        size_t                                               height_;
        size_t                                               width_;
        std::unique_ptr<EntityHandle<Schema>[]>              objects_;
        std::unique_ptr<std::vector<EntityHandle<Schema>>[]> properties_;
    };

}
