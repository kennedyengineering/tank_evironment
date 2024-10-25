// Tank Game (@kennedyengineering)

#include "engine.h"
#include "render.h"

#include <box2d/box2d.h>
#include <stdio.h>

#define TIME_STEP   1.0f / 60.0f
#define SUB_STEPS   4

static bool initialized = false;

static b2WorldId worldId;

bool engineInit()
{
    // Allocate resources
    if (initialized)
        return initialized;

    if (renderInit() == false)
    {
        fprintf(stderr, "Failed to initialize render method\n");
        return initialized;
    }

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -10.0f};

    worldId = b2CreateWorld(&worldDef);
    
    initialized = true;

    return initialized;
}

static b2BodyId engineCreateTank(b2Vec2 position, float angle)
{
    // Create a tank
    if (!initialized)
        return (b2BodyId){0};

    // Create the body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.rotation = b2MakeRot(angle);
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.3f;

    // Add the center shape
    float height, width;
    height = 7.93f;
    width = 3.66f;
    b2Polygon centerPolygon = b2MakeBox(height, width);
    b2CreatePolygonShape(bodyId, &shapeDef, &centerPolygon);

    return bodyId;
}

void engineStep()
{
    // Step forward the physics engine
    if (!initialized)
        return;

    b2World_Step(worldId, TIME_STEP, SUB_STEPS);
}

void engineRender()
{
    // Render the physics engine
    if (!initialized)
        return;

    renderCircle((float[]){0.0f, 0.0f}, 0.2f, (float[]){0.5f, 0.2f, 0.0f});
}

void engineDestroy()
{
    // Deallocate resources
    if (!initialized)
        return;

    renderDestroy();

    b2DestroyWorld(worldId);

    initialized = false;
}
