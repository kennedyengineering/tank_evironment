// Tank Game (@kennedyengineering)

#include "engine.h"

static bool initialized = false;

bool engineInit()
{
    // Allocate resources
    if (initialized)
        return initialized;

    // stuff ...
    
    initialized = true;

    return initialized;
}

void engineStep()
{
    // Step forward the physics engine
    if (!initialized)
        return;

    // stuff ...
}

void engineDestroy()
{
    // Deallocate resources
    if (!initialized)
        return;

    // stuff ...

    initialized = false;
}
