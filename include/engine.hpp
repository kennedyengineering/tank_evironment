// Tank Game (@kennedyengineering)

#pragma once

#include <box2d/box2d.h>
#include <filesystem>
#include <vector>

#include "categories.hpp"
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
  void moveLeftTankTread(RegistryId tankId, float speed);
  void moveRightTankTread(RegistryId tankId, float speed);

  std::vector<float> scanTankLidar(RegistryId tankId);
  float getTankGunAngle(RegistryId tankId);
  std::pair<float, float> getTankPosition(RegistryId tankId);
  std::pair<float, float> getTankWorldVelocity(RegistryId tankId);
  std::pair<float, float> getTankLocalVelocity(RegistryId tankId);

  void clearImage();

  void renderProjectiles();
  void renderTank(RegistryId tankId);
  void renderTankLidar(RegistryId tankId);

  std::vector<unsigned char> getImageBuffer();
  std::pair<int, int> getImageDimensions();
  int getImageChannels();
  void writeImageToPng(const std::filesystem::path &filePath);

  std::vector<std::tuple<CategoryBits, RegistryId, RegistryId>> step();

private:
  std::vector<std::tuple<CategoryBits, RegistryId, RegistryId>>
  handleCollisions();

private:
  Config mConfig;

  b2WorldId mWorldId;

  RenderEngine mRenderEngine;

  Registry<Tank> mTankRegistry;

  std::vector<b2ShapeId> mProjectileShapeIdVector;
};
} // namespace TankGame
