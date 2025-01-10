// Tank Game (@kennedyengineering)

#include <gtest/gtest.h>

#include "config.hpp"

TEST(ConfigTest, TankConfigInitialization) {
  // Ensure default initialization works
  TankGame::TankConfig tankConfig;
}

// TODO: add TankConfig::ValidateConfig

TEST(ConfigTest, ConfigInitialization) {
  // Ensure default initialization works
  TankGame::Config config;
}

// TODO: add Config::ValidateConfig
