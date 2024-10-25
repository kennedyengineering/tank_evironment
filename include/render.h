// Tank Game (@kennedyengineering)

#pragma once

#include <glad/gl.h>

#include <stdbool.h>

bool renderInit();

void renderPolygon(GLfloat vertices[], GLsizei count, GLfloat color[]);

void renderCircle(GLfloat center[], GLfloat radius, GLfloat color[]);

void renderDestroy();
