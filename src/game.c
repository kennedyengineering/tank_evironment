// Tank Game (@kennedyengineering)

#include "game.h"
#include "engine.h"

#include <stdio.h>

static bool initialized = false;

bool gameInit()
{
    // Allocate resources
    if (initialized)
        return initialized;

    if (engineInit() == false)
    {
        fprintf(stderr, "Failed to initialize physics engine\n");
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

    engineRender();
}

void gameDestroy()
{
    // Deallocate resources
    if (!initialized)
        return;

    engineDestroy();

    initialized = false;
}
