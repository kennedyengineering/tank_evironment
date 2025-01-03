// Tank Game (@kennedyengineering)

#pragma once

namespace TankGame
{
    struct TankConfig
    {
        /* Initialization parmameters */
        float positionX = 0.0f;    // in meters
        float positionY = 0.0f;    // in meters
        float angle = 0.0f;        // in radians

        /* Tank dimensions (M1-Abrams) */
        float bodyHeight = 7.93f;  // in meters
        float bodyWidth = 3.66f;   // in meters
        float gunHeight = 5.805f;  // in meters
        float gunWidth = 0.20f;    // in meters
        float treadWidth = 0.40f;  // in meters

        /* Projectile parameters */
        float projectileVelocity = 30.0f;   // in meters per second

        /* Lidar parameters */
        unsigned int lidarPoints = 360;     // number of lidar points
    };

    struct Config
    {
        /* Arena dimensions */
        unsigned int arenaWidth = 100;    // in meters
        unsigned int arenaHeight = 75;    // in meters

        /* Simulation parameters */
        float timeStep = 1.0f / 60.0f;  // in seconds
        int subStep = 8;                // number of sub-steps
    };
}