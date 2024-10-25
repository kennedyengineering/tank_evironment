// Tank Game (@kennedyengineering)

#include "engine.h"
#include "render.h"

#include <box2d/box2d.h>
#include <stdio.h>

#define TIME_STEP   1.0f / 60.0f
#define SUB_STEPS   4.0f

#define ARENA_HEIGHT 48.8f   // football field (meters)
#define ARENA_WIDTH  109.7f

#define TANK_HEIGHT 7.93f   // m1-abrams (meters)
#define TANK_WIDTH  3.66f

static bool initialized = false;

static b2WorldId worldId;
static b2DebugDraw debugDraw;

typedef struct RGBf
{
	float r, g, b;
} RGBf;

static inline RGBf MakeRGBf(b2HexColor c)
{
    // Convert a box2d color to RGBf struct
	return (RGBf){(float)((( c >> 16) & 0xFF) / 255.0f), (float)(((c >> 8) & 0xFF) / 255.0f), (float)((c & 0xFF) / 255.0f)};
}

static void DrawSolidPolygon (b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void* context)
{
    // Render a box2d polygon
    GLfloat gl_vertices[vertexCount*2];

    for (int i = 0; i < vertexCount; i++) {
        gl_vertices[2 * i] = (vertices[i].x+transform.p.x) / ARENA_WIDTH;       // x coordinate
        gl_vertices[2 * i + 1] = (vertices[i].y+transform.p.y) / ARENA_HEIGHT;  // y coordinate
    }

    RGBf colorf = MakeRGBf(color);

    renderPolygon(gl_vertices, vertexCount, (float[]){colorf.r, colorf.g, colorf.b});
}

static b2BodyId engineCreateTank(b2Vec2 position, float angle)
{
    // Create a tank
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.rotation = b2MakeRot(angle);
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.customColor = b2_colorGreenYellow;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.3f;

    b2Polygon centerPolygon = b2MakeBox(TANK_HEIGHT, TANK_WIDTH);
    b2CreatePolygonShape(bodyId, &shapeDef, &centerPolygon);

    return bodyId;
}

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
    worldDef.gravity = (b2Vec2){0.0f, 0.0f};

    worldId = b2CreateWorld(&worldDef);

    debugDraw.drawShapes = true;
    debugDraw.DrawSolidPolygon = &DrawSolidPolygon;

    engineCreateTank((b2Vec2){1.0f, 0.0f}, 0.0f);
    
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

void engineRender()
{
    // Render the physics engine
    if (!initialized)
        return;

    // renderCircle((float[]){0.0f, 0.0f}, 0.2f, (float[]){0.5f, 0.2f, 0.0f});
    b2World_Draw(worldId, &debugDraw);
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
