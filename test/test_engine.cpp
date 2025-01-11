// Tank Game (@kennedyengineering)

#include <gtest/gtest.h>

#include "config.hpp"
#include "engine.hpp"

TEST(EngineTest, Initialization) {
  // Ensure Engine initializes correctly

  // Create config
  TankGame::Config config;

  // Create engine
  TankGame::Engine eng(config);
}

TEST(EngineTest, RenderTankToPng) {
  // Ensure Engine renders correctly

  // Create config
  TankGame::Config config;

  // Create engine
  TankGame::Engine eng(config);

  // Create tank config
  TankGame::TankConfig tankConfig;
  tankConfig.positionX = config.arenaWidth / 2.0f;
  tankConfig.positionY = config.arenaHeight / 2.0f;

  // Create tank
  TankGame::RegistryId id = eng.addTank(tankConfig);

  // Render tank
  eng.renderTank(id);

  // Save to PNG
  eng.writeImageToPng("EngineTest_RenderTankToPng.png");
}
