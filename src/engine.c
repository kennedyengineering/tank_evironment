// Tank Game (@kennedyengineering)

#include "engine.h"

#include <box2d/box2d.h>

#define TIME_STEP   1.0f / 60.0f
#define SUB_STEPS   4

static bool initialized = false;

static b2WorldId worldId;

bool engineInit()
{
    // Allocate resources
    if (initialized)
        return initialized;

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -10.0f};

    worldId = b2CreateWorld(&worldDef);
    
    initialized = true;

    return initialized;
}

void engineStep()
{
    // Step forward the physics engine
    if (!initialized)
        return;

    b2World_Step(worldId, TIME_STEP, SUB_STEPS);
}

void engineDestroy()
{
    // Deallocate resources
    if (!initialized)
        return;

    b2DestroyWorld(worldId);

    initialized = false;
}
