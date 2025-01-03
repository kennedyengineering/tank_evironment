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
    
    private:
        TankConfig mTankConfig;

        b2WorldId mWorldId;

        b2BodyId mTankBodyId;
        b2BodyId mGunBodyId;
        b2BodyId mLeftTreadBodyId;
        b2BodyId mRightTreadBodyId;

        b2JointId mGunMotorJointId;
    };
}
