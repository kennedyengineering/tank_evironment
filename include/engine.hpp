// Tank Game (@kennedyengineering)

#pragma once

#include <box2d/box2d.h>

namespace TankGame
{
    class Engine
    {
    public:
        Engine();
        ~Engine();

        void step();
        void destroy();

    private:
        b2WorldId mWorldId;
    };
}
