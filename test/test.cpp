#include <gtest/gtest.h>

#include "engine.hpp"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {

    TankGame::Engine eng = TankGame::Engine(TankGame::Config());

    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}
