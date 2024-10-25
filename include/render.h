// Tank Game (@kennedyengineering)

#pragma once

#include <glad/gl.h>

#include <stdbool.h>

bool renderInit();

void renderPolygon(float *vertices, int count);

void renderDestroy();
