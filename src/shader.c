// Tank Game (@kennedyengineering)

#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

static void sPrintShaderError(GLuint shader)
{
    // Print OpenGL shader error
    if (glIsShader(shader) == GL_FALSE)
    {
        return;
    }

    GLint log_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

    char* log = (char*)malloc(log_length);
    glGetShaderInfoLog(shader, log_length, NULL, log);

    fprintf(stderr, "Shader Error: %s", log);

    free(log);
}

static void sPrintProgramError(GLuint program)
{
    // Print OpenGL shader error
    if (glIsProgram(program) == GL_FALSE)
    {
        return;
    }

    GLint log_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

    char* log = (char*)malloc(log_length);
    glGetProgramInfoLog(program, log_length, NULL, log);

    fprintf(stderr, "Program Error: %s", log);

    free(log);
}

static GLuint sCreateShaderFromString(const char* source, GLenum type)
{
    // Create a shader from a string
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        fprintf(stderr, "Error compiling shader of type %d\n", type);
        sPrintShaderError(shader);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint createProgramFromStrings(const char* vertexString, const char* fragmentString)
{
    // Create a shader program from a string
    GLuint vertexShader = sCreateShaderFromString(vertexString, GL_VERTEX_SHADER);
    if (vertexShader == 0)
    {
        return 0;
    }

    GLuint fragmentShader = sCreateShaderFromString(fragmentString, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0)
    {
        return 0;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        fprintf(stderr, "Error linking shader program\n");
        sPrintProgramError(shaderProgram);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint createProgramFromFiles(const char* vertexPath, const char* fragmentPath)
{
    // Create a shader program from a file

}
