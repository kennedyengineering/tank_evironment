// Tank Game (@kennedyengineering)

#pragma once

// TODO: figure out how to make this scalable with more than two tanks. Maybe use new mechanism for collision resolution?
namespace TankGame
{
    enum CategoryBits
    {
        PROJECTILE = 0x00000008,
        WALL       = 0x00000004,
        TANK1      = 0x00000002,
        TANK2      = 0x00000001,
    };
}
