#pragma once

#include <memory>
#include "entity/entity_database.h"
#include "schema.h"

namespace entler {

    class Scene : public EntityObserver<Schema> {
        using EntityHandles = std::unique_ptr<EntityHandle<Schema>[]>;

    public:
        Scene(EntityDatabase<Schema>& database, size_t width, size_t height)
            : EntityObserver<Schema>(database)
            , width_(width)
            , height_(height)
            , objects_(new EntityHandle<Schema>[width * height])
            , properties_(new EntityHandle<Schema>[width * height])
        {
        }

        void move_object(Entity<Schema> entity, I32Vec3 new_position) {
            auto& position = entity.get_component<ComponentType::position>();
            auto old_offset = get_offset(position.value);
            auto new_offset = get_offset(new_position);

            assert(entity.has_component<ComponentType::object_type>());
            assert(objects_[old_offset]);
            assert(!objects_[new_offset]);

            objects_[new_offset] = std::move(objects_[old_offset]);
            position.value = new_position;
        }

        void entity_added(Entity<Schema> entity) override {
            if (entity.has_component<ComponentType::position>()) {
                return;
            }

            auto& position = entity.get_component<ComponentType::position>();
            auto offset = get_offset(position.value);

            if (entity.has_component<ComponentType::object_type>()) {
                assert(!objects_[offset]);
                objects_[offset] = EntityHandle<Schema>(entity);
            }
            if (entity.has_component<ComponentType::property_type>()) {
                assert(!properties_[offset]);
                properties_[offset] = EntityHandle<Schema>(entity);
            }
        }

    private:
        size_t get_offset(I32Vec3 position) {
            return position.x + (position.y * width_);
        }

    private:
        size_t        height_;
        size_t        width_;
        EntityHandles objects_;
        EntityHandles properties_;
    };

}
