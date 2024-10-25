// Tank Game (@kennedyengineering)

#include "game.h"
#include "render.h"
#include "engine.h"

#include <stdio.h>

static bool initialized = false;

bool gameInit()
{
    // Allocate resources
    if (initialized)
        return initialized;

    // Initialize render method
    if (renderInit() == false)
    {
        fprintf(stderr, "Failed to initialize render method\n");
        return initialized;
    }

    // Initialize physics engine
    if (engineInit() == false)
    {
        fprintf(stderr, "Failed to initialize physics engine\n");
        renderDestroy();
        return initialized;
    }

    initialized = true;

    return initialized;
}

void gameStep()
{
    // Step forward the game
    if (!initialized)
        return;

    engineStep();
}

void gameRender()
{
    // Render the game
    if (!initialized)
        return;

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

    renderPolygon(vertices, 5, color_R);
    renderCircle(center, radius, color_G);
}

void gameDestroy()
{
    // Deallocate resources
    if (!initialized)
        return;

    renderDestroy();
    engineDestroy();

    initialized = false;
}
