// Tank Game (@kennedyengineering)

#pragma once

#include <box2d/box2d.h>

namespace TankGame {
struct ObstacleConfig {
  /* Initialization parameters */
  float positionX = 0.0f; // in meters
  float positionY = 0.0f; // in meters
  float radius = 0.0f;    // in meters

  /* Rendering parameters */
  b2HexColor color = b2_colorKhaki;
};

struct TankConfig {
  /* Initialization parameters */
  float positionX = 0.0f; // in meters
  float positionY = 0.0f; // in meters
  float angle = 0.0f;     // in radians

  /* Tank dimensions (M1-Abrams) */
  float bodyHeight = 7.93f;  // in meters
  float bodyWidth = 3.66f;   // in meters
  float gunHeight = 5.805f;  // in meters
  float gunWidth = 0.20f;    // in meters
  float treadHeight = 7.93f; // in meters
  float treadWidth = 0.40f;  // in meters

  /* Tread parameters */
  float treadMaxSpeed = 20.0f;

  /* Projectile parameters */
  float projectileVelocity = 30.0f; // in meters per second

  /* Lidar parameters */
  unsigned int lidarPoints = 360; // number of lidar points
  float lidarRange = 20;          // in meters

  /* Gun parameters */
  float gunDensity = 0.001f;
  float gunAngleMax = b2_pi / 4.0f;  // in radians
  float gunAngleMin = -b2_pi / 4.0f; // in radians

  /* Gun motor parameters */
  float gunMotorMaxForce = 5.0f;
  float gunMotorMaxTorque = 20.0f;
  float gunMotorCorrectionFactor = 0.05;

  /* Rendering parameters */
  b2HexColor projectileColor = b2_colorGray;    // hex color
  b2HexColor lidarColor = b2_colorGold;         // hex color
  b2HexColor tankColor = b2_colorYellow;        // hex color
  b2HexColor gunColor = b2_colorLime;           // hex color
  b2HexColor leftTreadColor = b2_colorCyan;     // hex color
  b2HexColor rightTreadColor = b2_colorMagenta; // hex color

  float lidarRadius = 3.0f; // in pixels
};

struct Config {
  /* Arena dimensions */
  float arenaWidth = 100.0f; // in meters
  float arenaHeight = 75.0f; // in meters

  /* Arena wall dimensions */
  float arenaWallThickness = 10.0f;

  /* Simulation parameters */
  float timeStep = 1.0f / 60.0f; // in seconds
  int subStep = 8;               // number of sub-steps

  /* Rendering parameters */
  b2HexColor clearColor = b2_colorBlack; // hex color

  float pixelDensity = 8.0f; // pixels per meter

  /* Logging */
  bool verboseOutput = false;
};
} // namespace TankGame
