// Tank Game (@kennedyengineering)

/* Includes */
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdbool.h>

#include "render.h"

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

    // Initialize render method
    if (renderInit() == false)
    {
        fprintf(stderr, "Failed to initialize render method\n");
        glfwTerminate();
        return RETURN_ERROR;
    }

    GLfloat vertices[] = {
         0.5f,  0.5f,  // top right
         0.5f, -0.5f,  // bottom right
        -0.5f, -0.5f,  // bottom left
        -0.5f,  0.5f,  // top left
        0.0f,  0.75f,  // tip
    };

    GLfloat color_R[] = {
        1.0f, 0.0f, 0.0f,
    };

    GLfloat color_G[] = {
        0.0f, 1.0f, 0.0f,
    };

    GLfloat center[] = {
        0.25f, 0.25f,
    };

    GLfloat radius = 0.2;

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Render
        glClear(GL_COLOR_BUFFER_BIT);
        renderPolygon(vertices, 5, color_R);
        renderCircle(center, radius, color_G);

        // Update
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    renderDestroy();

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
