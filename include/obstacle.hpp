// Tank Game (@kennedyengineering)

#pragma once

#include <box2d/box2d.h>
#include <vector>

#include "config.hpp"

namespace TankGame {
using ObstacleId = unsigned int;

class Obstacle {
public:
  Obstacle(ObstacleId obstacleId, const ObstacleConfig &obstacleConfig,
           b2WorldId worldId);
  ~Obstacle();

  b2Vec2 getPosition();
  float getRadius();

  std::vector<std::pair<b2ShapeId, b2HexColor>> getShapeIdsAndColors();

private:
  ObstacleId mObstacleId;

  ObstacleConfig mObstacleConfig;

  b2WorldId mWorldId;

  b2BodyId mObstacleBodyId;
  b2ShapeId mObstacleShapeId;
};
} // namespace TankGame
