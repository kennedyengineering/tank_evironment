// Tank Game (@kennedyengineering)

#pragma once

#include <box2d/box2d.h>

namespace TankGame
{
    struct TankConfig
    {
        /* Initialization parameters */
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

        /* Gun parameters */
        float gunDensity = 0.001f;

        /* Gun motor parameters */
        float gunMotorMaxForce = 5.0f;
        float gunMotorMaxTorque = 20.0f;
        float gunMotorCorrectionFactor = 0.05;

        /* Rendering parameters */
        b2HexColor projectileColor = b2_colorGray;      // hex color
        b2HexColor lidarColor = b2_colorGold;           // hex color
        b2HexColor tankColor = b2_colorYellow;          // hex color
        b2HexColor gunColor = b2_colorLime;             // hex color
        b2HexColor leftTreadColor = b2_colorCyan;       // hex color
        b2HexColor rightTreadColor = b2_colorMagenta;   // hex color

        float lidarRadius = 5;      // in pixels
    };

    struct Config
    {
        /* Arena dimensions */
        unsigned int arenaWidth = 100;    // in meters
        unsigned int arenaHeight = 75;    // in meters

        /* Simulation parameters */
        float timeStep = 1.0f / 60.0f;  // in seconds
        int subStep = 8;                // number of sub-steps

        /* Rendering parameters */
        b2HexColor clearColor = b2_colorBlack;  // hex color

        float pixelDensity = 8.0f;     // pixels per meter
    };
}
