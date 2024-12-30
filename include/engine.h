// Tank Game (@kennedyengineering)

#pragma once

#include <stdbool.h>

typedef struct
{
    float gun_angle;
    bool fire_gun;
    float tread_force[2];
} TankAction;

bool engineInit();

void engineStep(TankAction t1a, TankAction t2a);

void engineRender();

void engineDestroy();
