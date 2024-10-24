// Tank Game (@kennedyengineering)

#pragma once

#include <glad/gl.h>

GLuint createProgramFromStrings(const char* vertexString, const char* fragmentString);
GLuint createProgramFromFiles(const char* vertexPath, const char* fragmentPath);
