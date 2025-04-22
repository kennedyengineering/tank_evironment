// Tank Game (@kennedyengineering)

#pragma once

#include <cstdint>

namespace TankGame {
enum CategoryBits : uint32_t {
  OBSTACLE = 0x00000010,
  PROJECTILE = 0x00000008,
  WALL = 0x00000004,
  TANK_BODY = 0x00000002,
  TANK_GUN = 0x00000001,
  ALL = 0xFFFFFFFF
};
}
