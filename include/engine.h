// Tank Game (@kennedyengineering)

#pragma once

#include <stdbool.h>

typedef enum
{
    MODE_FORCE_TREAD,
    MODE_LIN_ROT_VELOCITY
} TankControlMode;

typedef struct TankAction
{
    float gun_angle;
    bool fire_gun;
    float angular_velocity;
    float linear_velocity[2];
    float tread_force[2];
    TankControlMode control_mode;
} TankAction;

bool engineInit();

void engineStep(TankAction t1a, TankAction t2a);

void engineRender();

void engineDestroy();
