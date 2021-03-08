#pragma once

#include "entity/entity_database.h"
#include "schema.h"
#include "scene.h"

namespace entler {

    class Simulation {
    public:
        Simulation(size_t width, size_t height);

    private:
        Scene                  scene_;
        EntityDatabase<Schema> database_;
    };

}
