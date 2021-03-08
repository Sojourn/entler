#pragma once

#include <memory>
#include "entity/entity_database.h"
#include "schema.h"

namespace entler {

    class Scene : public EntityObserver<Schema> {
    public:
        Scene(EntityDatabase<Schema>& database)
            : EntityObserver<Schema>(database)
        {
        }

        void entity_added(Entity<Schema> entity) override {
            // TODO
        }

        void entity_removed(Entity<Schema> entity) override {
            // TODO
        }
    };

}
