// Tank Game (@kennedyengineering)

#pragma once

#include <box2d/box2d.h>

#include "config.hpp"

namespace TankGame
{
    class Tank
    {
    public:
        Tank(const TankConfig& tankConfig, b2WorldId worldId);
        ~Tank();

        // void rotateTurret(float angle);
        // void fireGun();
        // void moveLeftTread(float force);
        // void moveRightTread(float force);
        // void scanLidar();
    };
}
