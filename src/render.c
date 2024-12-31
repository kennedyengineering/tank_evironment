// Tank Game (@kennedyengineering)

// TODO: pre-allocate buffer with a MAX_VERTICES_COUNT number of vertices in GL_DYNAMIC_DRAW memory

#include "render.h"
#include "shader.h"

#include <stdio.h>
#include <stddef.h>
#include <math.h>

#define VERTEX_SHADER_FILE    "../data/polygon.vs"
#define FRAGMENT_SHADER_FILE  "../data/polygon.fs"

#define CIRCLE_VERTICES       20

static bool initialized = false;

static GLuint VBO, VAO, shaderProgram;

bool renderInit()
{
    // Allocate resources
    if (initialized)
        return initialized;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    shaderProgram = createProgramFromFiles(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE);
    if (shaderProgram == 0)
    {
        fprintf(stderr, "Failed to initialize OpenGL shaders\n");
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        return initialized;
    }

    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), NULL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    initialized = true;

    return initialized;
}

void renderPolygon(GLfloat vertices[], GLsizei count, GLfloat color[])
{
    // Render a polygon
    if (!initialized)
        return;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 2*count*sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    GLint colorLocation = glGetUniformLocation(shaderProgram, "color");
    glProgramUniform3fv(shaderProgram, colorLocation, 1, color);

    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLE_FAN, 0, count);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderCircle(GLfloat center[], GLfloat radius, GLfloat color[])
{
    // Render a circle
    if (!initialized)
        return;

    GLfloat vertices[2*CIRCLE_VERTICES] = {0};

    float angle_increment = 2 * M_PI / CIRCLE_VERTICES;

    for (int i = 0; i < CIRCLE_VERTICES; i++) {
        float theta = i * angle_increment;
        vertices[2 * i] = radius*cos(theta) + center[0];     // x coordinate
        vertices[2 * i + 1] = radius*sin(theta) + center[1]; // y coordinate
    }
    
    renderPolygon(vertices, CIRCLE_VERTICES, color);
}

void renderDestroy()
{
    // Deallocate resources
    if (!initialized)
        return;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    initialized = false;
}
