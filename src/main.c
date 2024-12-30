// Tank Game (@kennedyengineering)

/* Includes */
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdbool.h>

#include "game.h"
#include "engine.h"

/* Defines */
#define SCREEN_NAME   "Tank Game"
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define RETURN_ERROR -1
#define RETURN_NORM   0

#define TANK_TREAD_FORCE 1000.0f
#define TANK_GUN_ANGLE_INCREMENT 0.05f

static TankAction t1a, t2a;

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
        gameStep(t1a, t2a);

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
    
    // Tank 1 force controls

    t1a.tread_force[1] = 0.0f;
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        t1a.tread_force[1] += TANK_TREAD_FORCE;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        t1a.tread_force[1] -= TANK_TREAD_FORCE;

    t1a.tread_force[0] = 0.0f;
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        t1a.tread_force[0] += TANK_TREAD_FORCE;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        t1a.tread_force[0] -= TANK_TREAD_FORCE;

    if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        t1a.gun_angle += TANK_GUN_ANGLE_INCREMENT;
    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        t1a.gun_angle -= TANK_GUN_ANGLE_INCREMENT;

    // Tank 1 gun firing controls
    static bool t1_fired = false;
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        if (!t1_fired)
        {
            t1a.fire_gun = true;
            t1_fired = true;
        }
        else
            t1a.fire_gun = false;
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        t1a.fire_gun = false;
        t1_fired = false;
    }

    // Tank 2 force controls

    t2a.tread_force[1] = 0.0f;
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        t2a.tread_force[1] += TANK_TREAD_FORCE;
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        t2a.tread_force[1] -= TANK_TREAD_FORCE;

    t2a.tread_force[0] = 0.0f;
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        t2a.tread_force[0] += TANK_TREAD_FORCE;
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        t2a.tread_force[0] -= TANK_TREAD_FORCE;

    if(glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        t2a.gun_angle += TANK_GUN_ANGLE_INCREMENT;
    if(glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        t2a.gun_angle -= TANK_GUN_ANGLE_INCREMENT;

    // Tank 2 gun firing controls
    static bool t2_fired = false;
    if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        if (!t2_fired)
        {
            t2a.fire_gun = true;
            t2_fired = true;
        }
        else
            t2a.fire_gun = false;
    if(glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        t2a.fire_gun = false;
        t2_fired = false;
    }
}
