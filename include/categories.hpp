// Tank Game (@kennedyengineering)

#pragma once

namespace TankGame {
enum CategoryBits {
  PROJECTILE = 0x00000004,
  WALL = 0x00000002,
  TANK = 0x00000001,
  ALL = 0xFFFFFFFF
};
}
