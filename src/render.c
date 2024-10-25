// Tank Game (@kennedyengineering)

// TODO: add rendering for circles
// TODO: add move this file to renderPolygon, then add a meta render file that instantiates polygon and circle rendering

#include "render.h"
#include "shader.h"

#include <stdio.h>
#include <stddef.h>

#define VERTEX_SHADER_FILE    "../data/polygon.vs"
#define FRAGMENT_SHADER_FILE  "../data/polygon.fs"

static bool initialized = false;

static GLuint VBO, VAO, shaderProgram;

bool renderInit()
{
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

void renderPolygon(GLfloat vertices[], GLfloat color[], GLsizei count)
{
    // Render a polygon
    if (!initialized)
        return;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 2*count*sizeof(GLfloat), vertices, GL_STATIC_DRAW);   // TODO: pre-allocate buffer with a MAX_VERTICES_COUNT number of vertices in GL_DYNAMIC_DRAW memory

    GLint colorLocation = glGetUniformLocation(shaderProgram, "color");
    glProgramUniform3fv(shaderProgram, colorLocation, 1, color);

    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLE_FAN, 0, count);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
