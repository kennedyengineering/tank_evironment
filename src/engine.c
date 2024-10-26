// Tank Game (@kennedyengineering)

#include "engine.h"
#include "render.h"

#include <box2d/box2d.h>
#include <stdio.h>

#define TIME_STEP   1.0f / 60.0f
#define SUB_STEPS   4.0f

#define ARENA_HEIGHT 48.8f   // football field (meters)
#define ARENA_WIDTH  109.7f

#define TANK_BODY_HEIGHT 7.93f   // m1-abrams (meters)
#define TANK_BODY_WIDTH  3.66f
#define TANK_GUN_HEIGHT  5.805f
#define TANK_GUN_WIDTH   0.20f

static bool initialized = false;

static b2WorldId worldId;
static b2DebugDraw debugDraw;

typedef struct Tank
{
    b2BodyId bodyId, gunId;
    b2JointId motorId;
} Tank;

static Tank tank1, tank2;

static void RotateTankTurret(Tank tank, float angle)
{
    // Move the tank turret to a specific angle (in radians)
    b2MotorJoint_SetAngularOffset(tank.motorId, angle);
}

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
        b2Vec2 transformedPoint = b2TransformPoint(transform, vertices[i]);
        gl_vertices[2 * i] = transformedPoint.x / ARENA_WIDTH;
        gl_vertices[2 * i + 1] = transformedPoint.y / ARENA_HEIGHT;
    }

    RGBf colorf = MakeRGBf(color);

    renderPolygon(gl_vertices, vertexCount, (float[]){colorf.r, colorf.g, colorf.b});
}

static Tank engineCreateTank(b2Vec2 position, float angle)
{
    // Create a tank
    Tank tank;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.rotation = b2MakeRot(angle);
    tank.bodyId = b2CreateBody(worldId, &bodyDef);
    tank.gunId = b2CreateBody(worldId, &bodyDef);

    // Create the body shape
    b2ShapeDef bodyShapeDef = b2DefaultShapeDef();
    bodyShapeDef.density = 1.0f;
    bodyShapeDef.customColor = b2_colorGreenYellow;
    
    b2Polygon bodyPolygon = b2MakeBox(TANK_BODY_HEIGHT, TANK_BODY_WIDTH);
    b2CreatePolygonShape(tank.bodyId, &bodyShapeDef, &bodyPolygon);

    // Create the gun shape
    b2ShapeDef gunShapeDef = b2DefaultShapeDef();
    gunShapeDef.density = 0.001f;
    gunShapeDef.customColor = b2_colorGreen;

    b2Polygon gunPolygon = b2MakeOffsetBox(TANK_GUN_HEIGHT, TANK_GUN_WIDTH, (b2Vec2){TANK_GUN_HEIGHT, 0}, 0);
    b2CreatePolygonShape(tank.gunId, &gunShapeDef, &gunPolygon);

    // Create motor joint
    b2MotorJointDef jointDef = b2DefaultMotorJointDef();
    jointDef.bodyIdA = tank.bodyId;
    jointDef.bodyIdB = tank.gunId;
    jointDef.maxForce = 5.0f;           // strength the force keeping the two bodies together
    jointDef.maxTorque = 20.0f;         // strength of the motor
    jointDef.correctionFactor = 0.05;   // proportional control constant?

    tank.motorId = b2CreateMotorJoint(worldId, &jointDef);

    return tank;
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

    // Create tanks
    tank1 = engineCreateTank((b2Vec2){0.0f, 0.0f}, 0.0f);
    tank2 = engineCreateTank((b2Vec2){50.0f, 0.0f}, M_PI/4);

    RotateTankTurret(tank1, 1.0f);
    
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
