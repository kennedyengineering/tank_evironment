// Tank Game (@kennedyengineering)

#pragma once

#include <box2d/box2d.h>
#include <vector>

#include "config.hpp"
#include "tank.hpp"

namespace TankGame
{
    class Engine
    {
    public:
        Engine(const Config& config);
        ~Engine();

        void step();
        void destroy();

    private:
        b2WorldId mWorldId;
        std::vector<Tank> mTanks;
    };
}
