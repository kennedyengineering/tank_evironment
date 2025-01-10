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

class TestType {
public:
  TestType(int val) : mVal(val) { mConstructorCount++; }

  ~TestType() { mDestructorCount++; }

  int getVal() { return mVal; }

  void setVal(int val) { mVal = val; }

  static int getConstructorCount() { return mConstructorCount; }

  static int getDestructorCount() { return mDestructorCount; }

  static void resetCounters() {
    mConstructorCount = 0;
    mDestructorCount = 0;
  }

private:
  int mVal;
  static int mConstructorCount;
  static int mDestructorCount;
};

int TestType::mConstructorCount = 0;
int TestType::mDestructorCount = 0;

TEST(RegistryTest, ComplexTypeInitialization) {
  // Ensure registry works with complex data types

  // Reset counters
  TestType::resetCounters();

  // Construct with TestType type
  TankGame::Registry<TestType> reg;

  // Ensure ids are checked
  ASSERT_ANY_THROW(reg.get(0));
  ASSERT_ANY_THROW(reg.remove(0));

  // Check constructor count
  ASSERT_EQ(TestType::getConstructorCount(), 0);

  // Check destructor count
  ASSERT_EQ(TestType::getDestructorCount(), 0);
}

TEST(RegistryTest, ComplexTypeEmplaceGetRemove) {
  // Ensure registry works with complex data types

  // Reset counters
  TestType::resetCounters();

  // Construct with TestType type
  TankGame::Registry<TestType> reg;

  // Emplace TestType
  TankGame::RegistryId id = reg.emplace(2);

  // Retrieve TestType value
  ASSERT_EQ(reg.get(id).getVal(), 2);

  // Modify TestType value
  TestType &val = reg.get(id);
  val.setVal(3);
  ASSERT_EQ(reg.get(id).getVal(), 3);

  // Delete TestType
  reg.remove(id);

  // Check constructor count
  ASSERT_EQ(TestType::getConstructorCount(), 1);

  // Check destructor count
  ASSERT_EQ(TestType::getDestructorCount(), 1);
}

TEST(RegistryTest, ComplexTypeCountIds) {
  // Ensure registry works with complex data types

  // Reset counters
  TestType::resetCounters();

  // Construct with TestType type
  TankGame::Registry<TestType> reg;

  // Count Ids
  TankGame::RegistryId id1 = reg.emplace(1);
  ASSERT_EQ(id1, 0);
  TankGame::RegistryId id2 = reg.emplace(2);
  ASSERT_EQ(id2, 1);

  reg.remove(id1);

  TankGame::RegistryId id3 = reg.emplace(3);
  ASSERT_EQ(id3, 0);

  ASSERT_EQ(reg.get(0).getVal(), 3);
  ASSERT_EQ(reg.get(1).getVal(), 2);

  reg.remove(id2);
  reg.remove(id3);

  // Check constructor count
  ASSERT_EQ(TestType::getConstructorCount(), 3);

  // Check destructor count
  ASSERT_EQ(TestType::getDestructorCount(), 3);
}

TEST(RegistryTest, ComplexTypeEmplaceWithId) {
  // Ensure registry works with complex data types

  // Reset counters
  TestType::resetCounters();

  // Construct with TestType type
  TankGame::Registry<TestType> reg;

  // Emplace char with id
  TankGame::RegistryId id1 = reg.emplaceWithId();
  ASSERT_EQ(reg.get(id1).getVal(), 0);
  ASSERT_EQ(id1, 0);

  TankGame::RegistryId id2 = reg.emplaceWithId();
  ASSERT_EQ(reg.get(id2).getVal(), 1);
  ASSERT_EQ(id2, 1);

  reg.remove(id1);
  reg.remove(id2);

  // Check constructor count
  ASSERT_EQ(TestType::getConstructorCount(), 2);

  // Check destructor count
  ASSERT_EQ(TestType::getDestructorCount(), 2);
}
