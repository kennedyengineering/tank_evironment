// Tank Game (@kennedyengineering)

#pragma once

#include <box2d/box2d.h>
#include <vector>

#include "config.hpp"

namespace TankGame {
using TankId = unsigned int;

class Tank {
public:
  Tank(TankId tankId, const TankConfig &tankConfig, b2WorldId worldId);
  ~Tank();

  void rotateGun(float angle);
  b2ShapeId fireGun();
  void moveLeftTread(float speed);
  void moveRightTread(float speed);

  void scanLidar();
  void scanLidar(float range);

  std::vector<b2Vec2> getLidarData();
  b2Vec2 getPosition();
  b2Vec2 getWorldVelocity();
  b2Vec2 getLocalVelocity();
  float getGunAngle();
  b2HexColor getProjectileColor();
  b2HexColor getLidarColor();
  float getLidarRadius();
  std::vector<std::pair<b2ShapeId, b2HexColor>> getShapeIdsAndColors();

private:
  TankId mTankId;

  TankConfig mTankConfig;

  b2WorldId mWorldId;

  std::vector<b2Vec2> mLidarData;

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
} // namespace TankGame
