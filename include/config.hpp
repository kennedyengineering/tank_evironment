// Tank Game (@kennedyengineering)

#pragma once

#include <vector>

namespace TankGame
{
    struct TankConfig
    {
        
    };

    struct Config
    {
        unsigned int arenaWidth;
        unsigned int arenaHeight;

        std::vector<TankConfig> tankConfigs;
    };
}
