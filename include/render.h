// Tank Game (@kennedyengineering)

#pragma once

#include <glad/gl.h>

#include <stdbool.h>

bool renderInit();

void renderPolygon(float vertices[], float color[], int count);

void renderDestroy();
