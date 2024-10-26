// Tank Game (@kennedyengineering)

#pragma once

#include <stdbool.h>

typedef struct TankAction
{
    float gun_angle;
    float angular_velocity;
    float linear_velocity[2];
} TankAction;

bool engineInit();

void engineStep(TankAction t1a, TankAction t2a);

void engineRender();

void engineDestroy();
