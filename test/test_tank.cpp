// Tank Game (@kennedyengineering)

#include <box2d/box2d.h>
#include <gtest/gtest.h>

#include "config.hpp"
#include "tank.hpp"

TEST(TankTest, Initialization) {
  // Ensure tank initializes correctly

  // Initialize box2d world
  b2WorldDef worldDef = b2DefaultWorldDef();
  worldDef.gravity = (b2Vec2){0.0f, 0.0f};
  b2WorldId worldId = b2CreateWorld(&worldDef);

  // Create config
  TankGame::TankConfig config;

  // Create id
  TankGame::TankId id = 5;

  // Initialize tank
  TankGame::Tank tank(id, config, worldId);

  // Destroy box2d world
  b2DestroyWorld(worldId);
}
