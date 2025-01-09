// Tank Game (@kennedyengineering)

#pragma once

#include <box2d/box2d.h>
#include <vector>

#include "config.hpp"
#include "registry.hpp"
#include "render.hpp"
#include "tank.hpp"

namespace TankGame {
class Engine {
public:
  Engine(const Config &config);
  ~Engine();

  RegistryId addTank(const TankConfig &tankConfig);
  void removeTank(RegistryId tankId);

  void rotateTankGun(RegistryId tankId, float angle);
  void fireTankGun(RegistryId tankId);
  void moveLeftTankTread(RegistryId tankId, float force);
  void moveRightTankTread(RegistryId tankId, float force);

  std::vector<float> scanTankLidar(RegistryId tankId);

  void clearImage();

  void renderProjectiles();
  void renderTank(RegistryId tankId);
  void renderTankLidar(RegistryId tankId);

  std::vector<unsigned char> getImageBuffer();
  std::pair<int, int> getImageDimensions();
  int getImageChannels();

  void step();

private:
  void handleCollisions();

private:
  Config mConfig;

  b2WorldId mWorldId;

  RenderEngine mRenderEngine;

  Registry<Tank> mTankRegistry;

  std::vector<b2ShapeId> mProjectileShapeIdVector;
};
} // namespace TankGame
