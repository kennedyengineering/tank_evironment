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

TEST(EngineTest, RenderTankLidarToPng) {
  // Ensure Engine renders correctly

  // Create config
  TankGame::Config config;

  // Create engine
  TankGame::Engine eng(config);

  // Create tank config
  TankGame::TankConfig tankConfig1;
  tankConfig1.positionX = config.arenaWidth / 3.0f;
  tankConfig1.positionY = config.arenaHeight / 2.0f;

  // Create tank
  TankGame::RegistryId id1 = eng.addTank(tankConfig1);

  // Create other tank config
  TankGame::TankConfig tankConfig2;
  tankConfig2.positionX = 2.0f * config.arenaWidth / 3.0f;
  tankConfig2.positionY = config.arenaHeight / 2.0f;

  // Create other tank
  TankGame::RegistryId id2 = eng.addTank(tankConfig2);

  // Render tank
  eng.renderTank(id1);

  // Render other tank
  eng.renderTank(id2);

  // Render lidar
  eng.renderTankLidar(id1);

  // Save to PNG
  eng.writeImageToPng("EngineTest_RenderTankLidarToPng_PreScan.png");

  // Scan lidar
  eng.scanTankLidar(id1);

  // Render lidar
  eng.renderTankLidar(id1);

  // Save to PNG
  eng.writeImageToPng("EngineTest_RenderTankLidarToPng_PostScan.png");
}
