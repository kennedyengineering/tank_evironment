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

void gameStep(TankAction t1a, TankAction t2a)
{
    // Step forward the game
    if (!initialized)
        return;

    engineStep(t1a, t2a);
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
