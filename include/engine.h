// Tank Game (@kennedyengineering)

#pragma once

#include <stdbool.h>

typedef struct
{
    float gun_angle;
    bool fire_gun;
    float left_tread_force;
    float right_tread_force;
} TankAction;

bool engineInit();

void engineStep(TankAction t1a, TankAction t2a);

void engineDestroy();
