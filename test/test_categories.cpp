// Tank Game (@kennedyengineering)

#include <array>
#include <gtest/gtest.h>

#include "categories.hpp"

TEST(CategoriesTest, NoOverlap) {
  // Ensure no two category bits overlap
  std::array<TankGame::CategoryBits, 4> allCategoryBits = {
      TankGame::PROJECTILE, TankGame::WALL, TankGame::TANK_BODY,
      TankGame::TANK_GUN};

  uint32_t bitRecord = 0;

  for (const TankGame::CategoryBits &bits : allCategoryBits) {
    // Check if any bits overlap
    ASSERT_EQ(bitRecord & bits, 0);

    // Add bits to record
    bitRecord |= bits;
  }
}

TEST(CategoriesTest, AllContainsAll) {
  // Ensure ALL contains all bits in enum
  std::array<TankGame::CategoryBits, 4> allCategoryBits = {
      TankGame::PROJECTILE, TankGame::WALL, TankGame::TANK_BODY,
      TankGame::TANK_GUN};

  for (const TankGame::CategoryBits &bits : allCategoryBits) {
    // Check if all bits are covered
    ASSERT_EQ(TankGame::CategoryBits::ALL & bits, bits);
  }
}
