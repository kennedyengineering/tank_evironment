// Tank Game (@kennedyengineering)

/* Includes */
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdbool.h>

#include "shader.h"

/* Defines */
#define SCREEN_NAME   "Tank Game"
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define RETURN_ERROR -1
#define RETURN_NORM   0

/* Function declarations */
void error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

/* Program entry point */
int main() {

    // Initialize GLFW
    glfwSetErrorCallback(error_callback);
     if (glfwInit() == GLFW_FALSE)
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return RETURN_ERROR;
	}

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // TODO: make window non-resizeable via configuration (in the future)

    // Create GLFW window object
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_NAME, NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return RETURN_ERROR;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL function pointers
    if (gladLoadGL(glfwGetProcAddress) == 0)
    {
        fprintf(stderr, "Failed to initialize OpenGL context\n");
        glfwTerminate();
        return RETURN_ERROR;
    }

    // TODO: compile from file
    // Compile shader programs
    const char *vertexShaderSource = "#version 450 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char *fragmentShaderSource = "#version 450 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";
    createProgramFromStrings(vertexShaderSource, fragmentShaderSource);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Render
        glClear(GL_COLOR_BUFFER_BIT);

        // Update
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    // ...

    // Terminate
    glfwTerminate();
    return RETURN_NORM;
}

/* Function definitions */
void error_callback(int error, const char* description )
{
    // Display errors
	fprintf(stderr, "GLFW error occurred. Code: %d. Description: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);
} 

// TODO: replace with call back
// TODO: add TODO extension
void processInput(GLFWwindow *window)
{
    // Query GLFW whether relevant keys are pressed/released this frame and react accordingly
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
