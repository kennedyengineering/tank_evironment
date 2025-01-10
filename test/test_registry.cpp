// Tank Game (@kennedyengineering)

#include <gtest/gtest.h>

#include "registry.hpp"

TEST(RegistryTest, PrimitiveTypeInitialization) {
  // Ensure registry works with primitive data types

  // Construct with char type
  TankGame::Registry<char> reg;

  // Ensure ids are checked
  ASSERT_ANY_THROW(reg.get(0));
  ASSERT_ANY_THROW(reg.remove(0));
}

TEST(RegistryTest, PrimitiveTypeEmplaceGetRemove) {
  // Ensure registry works with primitive data types

  // Construct with char type
  TankGame::Registry<char> reg;

  // Emplace char
  TankGame::RegistryId id = reg.emplace(2);

  // Retrieve char value
  ASSERT_EQ(reg.get(id), 2);

  // Modify char value
  char &val = reg.get(id);
  val = 3;
  ASSERT_EQ(reg.get(id), 3);

  // Delete char
  reg.remove(id);
}

TEST(RegistryTest, PrimitiveTypeCountIds) {
  // Ensure registry works with primitive data types

  // Construct with char type
  TankGame::Registry<char> reg;

  // Count Ids
  TankGame::RegistryId id1 = reg.emplace(1);
  ASSERT_EQ(id1, 0);
  TankGame::RegistryId id2 = reg.emplace(2);
  ASSERT_EQ(id2, 1);

  reg.remove(id1);

  TankGame::RegistryId id3 = reg.emplace(3);
  ASSERT_EQ(id3, 0);

  ASSERT_EQ(reg.get(0), 3);
  ASSERT_EQ(reg.get(1), 2);

  reg.remove(id2);
  reg.remove(id3);
}

TEST(RegistryTest, PrimitiveTypeEmplaceWithId) {
  // Ensure registry works with primitive data types

  // Construct with char type
  TankGame::Registry<char> reg;

  // Emplace char with id
  TankGame::RegistryId id1 = reg.emplaceWithId();
  ASSERT_EQ(reg.get(id1), 0);
  ASSERT_EQ(id1, 0);

  TankGame::RegistryId id2 = reg.emplaceWithId();
  ASSERT_EQ(reg.get(id2), 1);
  ASSERT_EQ(id2, 1);

  reg.remove(id1);
  reg.remove(id2);
}

TEST(RegistryTest, ComplexType) {
  // Ensure registry constructs complex data types
}
