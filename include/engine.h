// Tank Game (@kennedyengineering)

#pragma once

#include <stdbool.h>
#include <stdint.h>

/* Rendering Constant */
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

typedef struct
{
    float gun_angle;
    bool fire_gun;
    float left_tread_force;
    float right_tread_force;
} TankAction;

bool engineInit();

void engineStep(TankAction t1a, TankAction t2a);

uint8_t *engineRender();

void engineDestroy();
