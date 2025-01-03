// Tank Game (@kennedyengineering)

#pragma once

#include <box2d/box2d.h>

#include "config.hpp"
#include "tank.hpp"
#include "registry.hpp"

namespace TankGame
{
    class Engine
    {
    public:
        Engine(const Config& config);
        ~Engine();

        RegistryId addTank(const TankConfig& tankConfig);
        void removeTank(RegistryId tankId);

        void step();
        void destroy();

    private:
        Config mConfig;

        b2WorldId mWorldId;

        Registry<Tank> mTankRegistry;
    };
}
