// Tank Game (@kennedyengineering)

#include "obstacle.hpp"
#include "categories.hpp"

using namespace TankGame;

Obstacle::Obstacle(ObstacleId obstacleId, const ObstacleConfig &obstacleConfig,
                   b2WorldId worldId)
    : mObstacleId(obstacleId), mObstacleConfig(obstacleConfig),
      mWorldId(worldId) {
  /* Create the obstacle */

  // Construct the body
  b2BodyDef bodyDef = b2DefaultBodyDef();
  bodyDef.type = b2_staticBody;

  mObstacleBodyId = b2CreateBody(mWorldId, &bodyDef);

  // Construct the shape
  b2ShapeDef bodyShapeDef = b2DefaultShapeDef();
  bodyShapeDef.filter.categoryBits = CategoryBits::OBSTACLE;
  bodyShapeDef.userData = &mObstacleId;

  b2Circle circleDef;
  circleDef.center =
      (b2Vec2){mObstacleConfig.positionX, mObstacleConfig.positionY};
  circleDef.radius = mObstacleConfig.radius;

  mObstacleShapeId =
      b2CreateCircleShape(mObstacleBodyId, &bodyShapeDef, &circleDef);
}

Obstacle::~Obstacle() {
  /* Destroy a obstacle */

  // Check if world has been destroyed
  if (!b2World_IsValid(mWorldId)) {
    return;
  }

  // Destroy the obstacle body
  b2DestroyBody(mObstacleBodyId);
}

b2Vec2 Obstacle::getPosition() {
  /* Get the obstacle position */

  // Return position
  return b2Shape_GetCircle(mObstacleShapeId).center;
};

float Obstacle::getRadius() {
  /* Get the obstacle radius */

  // Return radius
  return b2Shape_GetCircle(mObstacleShapeId).radius;
};

std::vector<std::pair<b2ShapeId, b2HexColor>> Obstacle::getShapeIdsAndColors() {
  /* Get shape and color of the obstacle (for rendering) */

  // Return shapes in order to be rendered
  return std::vector<std::pair<b2ShapeId, b2HexColor>>{
      std::pair<b2ShapeId, b2HexColor>{mObstacleShapeId,
                                       mObstacleConfig.color}};
};
