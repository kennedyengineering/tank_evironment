// Tank Game (@kennedyengineering)

#pragma once

#include <box2d/box2d.h>
#include <vector>

#include "config.hpp"

namespace TankGame
{
    using TankId = unsigned int;

    class Tank
    {
    public:
        Tank(TankId tankId, const TankConfig& tankConfig, b2WorldId worldId);
        ~Tank();

        void rotateGun(float angle);
        void fireGun();
        void moveLeftTread(float force);
        void moveRightTread(float force);

        std::vector<b2Vec2> scanLidar(float range);
    
    private:
        TankId mTankId;

        TankConfig mTankConfig;

        b2WorldId mWorldId;

        b2BodyId mTankBodyId;
        b2BodyId mGunBodyId;
        b2BodyId mLeftTreadBodyId;
        b2BodyId mRightTreadBodyId;

        b2JointId mGunMotorJointId;
    };
}
