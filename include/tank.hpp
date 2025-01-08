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
        b2ShapeId fireGun();
        void moveLeftTread(float force);
        void moveRightTread(float force);

        std::vector<b2Vec2> scanLidar(float range);

        b2HexColor getProjectileColor();
        b2HexColor getLidarColor();
        std::vector<std::pair<b2ShapeId, b2HexColor>> getShapeIdsAndColors();
    
    private:
        TankId mTankId;

        TankConfig mTankConfig;

        b2WorldId mWorldId;

        b2BodyId mTankBodyId;
        b2ShapeId mTankShapeId;

        b2BodyId mGunBodyId;
        b2ShapeId mGunShapeId;

        b2BodyId mLeftTreadBodyId;
        b2ShapeId mLeftTreadShapeId;

        b2BodyId mRightTreadBodyId;
        b2ShapeId mRightTreadShapeId;

        b2JointId mGunMotorJointId;
    };
}
