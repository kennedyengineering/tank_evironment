// Tank Game (@kennedyengineering)

/* Includes */
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdbool.h>

#include "game.h"

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

    // Initialize game
    if (gameInit() == false)
    {
        fprintf(stderr, "Failed to initialize game\n");
        glfwTerminate();
        return RETURN_ERROR;
    }

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Physics
        gameStep();

        // Render
        glClear(GL_COLOR_BUFFER_BIT);
        gameRender();

        // Update
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    gameDestroy();

    // Terminate
    glfwTerminate();
    return RETURN_NORM;
}

/* Function definitions */
void error_callback(int error, const char* description)
{
    // Display errors
	fprintf(stderr, "GLFW error occurred. Code: %d. Description: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);
} 

void processInput(GLFWwindow *window)
{
    // Query GLFW whether relevant keys are pressed/released this frame and react accordingly
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
