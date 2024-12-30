// Tank Game (@kennedyengineering)

#include "engine.h"
#include "render.h"

#include <box2d/box2d.h>
#include <stdio.h>


/* Simulation Constants */
#define TIME_STEP   1.0f / 60.0f
#define SUB_STEPS   4.0f


/* Arena Constants */
#define ARENA_WIDTH  100    // maintain aspect ratio of screen (meters)
#define ARENA_HEIGHT 75

#define GROUND_FRICTION_COEF 0.3

#define GRAVITY_ACCEL 9.8


/* Tank Constants */
#define TANK_BODY_HEIGHT 7.93f   // m1-abrams (meters)
#define TANK_BODY_WIDTH  3.66f
#define TANK_GUN_HEIGHT  5.805f
#define TANK_GUN_WIDTH   0.20f
#define TANK_TREAD_WIDTH 0.40f

#define TANK_PROJECTILE_VELOCITY 15.0f

#define TANK_LIDAR_POINTS 360


/* Global Variables*/
static bool initialized = false;

static b2WorldId worldId;

static b2DebugDraw debugDraw;

typedef enum
{
    PROJECTILE = 0x00000008,
    WALL       = 0x00000004,
    TANK1      = 0x00000002,
    TANK2      = 0x00000001,
} CategoryBits;


/* Engine Render Variables */
typedef struct
{
	float r, g, b;
} RGBf;


/* Engine Render Methods */
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


/* Tank Variables */
typedef struct
{
    b2BodyId bodyId, gunId, leftTreadId, rightTreadId;
    b2ShapeId leftTreadShapeId, rightTreadShapeId;
    b2JointId motorId;
    b2Vec2 lidarPoints[TANK_LIDAR_POINTS];
    uint32_t categoryBits;
} Tank;

static Tank tank1, tank2;


/* Tank Methods */
static Tank tankCreateNew(b2Vec2 position, float angle, uint32_t categoryBits)
{
    // Create a tank
    Tank tank = {0};

    tank.categoryBits = categoryBits;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.rotation = b2MakeRot(angle);
    tank.bodyId = b2CreateBody(worldId, &bodyDef);
    tank.gunId = b2CreateBody(worldId, &bodyDef);
    tank.leftTreadId = b2CreateBody(worldId, &bodyDef);
    tank.rightTreadId = b2CreateBody(worldId, &bodyDef);

    // Create the body shape
    b2ShapeDef bodyShapeDef = b2DefaultShapeDef();
    bodyShapeDef.customColor = b2_colorGreenYellow;
    bodyShapeDef.filter.categoryBits = categoryBits;
    
    b2Polygon bodyPolygon = b2MakeBox(TANK_BODY_HEIGHT, TANK_BODY_WIDTH);
    b2CreatePolygonShape(tank.bodyId, &bodyShapeDef, &bodyPolygon);

    // Create the left tread shape
    b2ShapeDef leftTreadShapeDef = b2DefaultShapeDef();
    leftTreadShapeDef.customColor = b2_colorHotPink;
    leftTreadShapeDef.filter.categoryBits = categoryBits;

    b2Polygon leftTreadPolygon = b2MakeOffsetBox(TANK_BODY_HEIGHT, TANK_TREAD_WIDTH, (b2Vec2){0, TANK_BODY_HEIGHT/2.0f}, 0);
    tank.leftTreadShapeId = b2CreatePolygonShape(tank.leftTreadId, &leftTreadShapeDef, &leftTreadPolygon);

    b2WeldJointDef leftTreadJointDef = b2DefaultWeldJointDef();
    leftTreadJointDef.bodyIdA = tank.bodyId;
    leftTreadJointDef.bodyIdB = tank.leftTreadId;
    b2CreateWeldJoint(worldId, &leftTreadJointDef);

    // Create the right tread shape
    b2ShapeDef rightTreadShapeDef = b2DefaultShapeDef();
    rightTreadShapeDef.customColor = b2_colorHotPink;
    rightTreadShapeDef.filter.categoryBits = categoryBits;

    b2Polygon rightTreadPolygon = b2MakeOffsetBox(TANK_BODY_HEIGHT, TANK_TREAD_WIDTH, (b2Vec2){0, -TANK_BODY_HEIGHT/2.0f}, 0);
    tank.rightTreadShapeId = b2CreatePolygonShape(tank.rightTreadId, &rightTreadShapeDef, &rightTreadPolygon);

    b2WeldJointDef rightTreadJointDef = b2DefaultWeldJointDef();
    rightTreadJointDef.bodyIdA = tank.bodyId;
    rightTreadJointDef.bodyIdB = tank.rightTreadId;
    b2CreateWeldJoint(worldId, &rightTreadJointDef);

    // Create the gun shape
    b2ShapeDef gunShapeDef = b2DefaultShapeDef();
    gunShapeDef.density = 0.001f;
    gunShapeDef.customColor = b2_colorGreen;
    gunShapeDef.filter.categoryBits = categoryBits;

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

static void tankRotateGun(Tank tank, float angle)
{
    // Move the tank gun to a specific angle relative to the tank body (in radians)
    b2MotorJoint_SetAngularOffset(tank.motorId, angle);
}

static void tankForceTreads(Tank tank, float force_left, float force_right)
{
    // TODO: Make movement less slide-y and more wheel-like.

    // Apply force to tank treads
    b2Vec2 leftTreadWorldForce = b2Body_GetWorldVector(tank.bodyId, (b2Vec2){force_left, 0});
    b2Body_ApplyForceToCenter(tank.leftTreadId, leftTreadWorldForce, true);

    b2Vec2 rightTreadWorldForce = b2Body_GetWorldVector(tank.bodyId, (b2Vec2){force_right, 0});
    b2Body_ApplyForceToCenter(tank.rightTreadId, rightTreadWorldForce, true);

    // Apply friction force to tank treads
    float mass = b2Body_GetMass(tank.bodyId) + b2Body_GetMass(tank.gunId) + b2Body_GetMass(tank.leftTreadId) + b2Body_GetMass(tank.rightTreadId);

    float left_friction_coef = sqrtf(GROUND_FRICTION_COEF * b2Shape_GetFriction(tank.leftTreadShapeId));
    float left_friction_force = left_friction_coef*mass*GRAVITY_ACCEL/2;    // divide by two because two treads share the load

    b2Vec2 leftTreadWorldNormalizedLinearVelocity = b2Normalize(b2Body_GetLinearVelocity(tank.leftTreadId));
    b2Vec2 leftTreadWorldFrictionForceVector = b2MulSV(left_friction_force, b2Neg(leftTreadWorldNormalizedLinearVelocity));

    b2Body_ApplyForceToCenter(tank.leftTreadId, leftTreadWorldFrictionForceVector, true);

    float right_friction_coef = sqrtf(GROUND_FRICTION_COEF * b2Shape_GetFriction(tank.rightTreadShapeId));
    float right_friction_force = right_friction_coef*mass*GRAVITY_ACCEL/2;  // divide by two because two treads share the load

    b2Vec2 rightTreadWorldNormalizedLinearVelocity = b2Normalize(b2Body_GetLinearVelocity(tank.rightTreadId));
    b2Vec2 rightTreadWorldFrictionForceVector = b2MulSV(right_friction_force, b2Neg(rightTreadWorldNormalizedLinearVelocity));

    b2Body_ApplyForceToCenter(tank.rightTreadId, rightTreadWorldFrictionForceVector, true);    
}

static void tankFireGun(Tank tank)
{
    // TODO: Initialize projectile inside of gun?

    // Create body
    b2BodyDef projectileBodyDef = b2DefaultBodyDef();
    projectileBodyDef.type = b2_dynamicBody;
    projectileBodyDef.position = b2Body_GetPosition(tank.gunId);
    projectileBodyDef.rotation = b2Body_GetRotation(tank.gunId);
    projectileBodyDef.linearVelocity = b2Body_GetWorldVector(tank.gunId, (b2Vec2){TANK_PROJECTILE_VELOCITY, 0.0f});
    b2BodyId projectileBodyId = b2CreateBody(worldId, &projectileBodyDef);

    // Create shape
    b2ShapeDef projectileShapeDef = b2DefaultShapeDef();
    projectileShapeDef.customColor = b2_colorGray;
    projectileShapeDef.filter.categoryBits = PROJECTILE;
    uint32_t maskBits = PROJECTILE | WALL | (tank.categoryBits == TANK1 ? TANK2 : TANK1);
    projectileShapeDef.filter.maskBits = maskBits;

    b2Polygon projectilePolygon = b2MakeOffsetBox(TANK_GUN_WIDTH, TANK_GUN_WIDTH, (b2Vec2){TANK_GUN_HEIGHT*2+TANK_GUN_WIDTH, 0}, 0);
    b2CreatePolygonShape(projectileBodyId, &projectileShapeDef, &projectilePolygon);
}
 
static float __rayCastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
{
    *(b2Vec2*)context = point;
    return fraction;
}

static void tankScanLidar(Tank *tank)
{
    // Update tank lidar information
    // https://box2d.org/documentation/md_simulation.html#autotoc_md115

    for (size_t cast_num = 0; cast_num < TANK_LIDAR_POINTS; cast_num++)
    {
        // Compute angle
        float d_angle = 2*b2_pi/TANK_LIDAR_POINTS;
        float angle = d_angle*cast_num;
        b2Rot rot = b2MakeRot(angle);

        // Perform ray-cast
        b2Vec2 start = b2Body_GetPosition(tank->bodyId);
        b2Vec2 end = b2Body_GetWorldPoint(tank->bodyId, b2RotateVector(rot, (b2Vec2){ARENA_WIDTH*ARENA_HEIGHT, 0}));
        b2Vec2 translation = b2Sub(end, start);

        b2Vec2 point = {0};
        uint32_t maskBits = PROJECTILE | WALL | (tank->categoryBits == TANK1 ? TANK2 : TANK1);
        b2QueryFilter viewFilter = {.categoryBits=0xFFFFFFFF, .maskBits=maskBits};
        b2World_CastRay(worldId, start, translation, viewFilter, __rayCastCallback, &point);

        // Update tank memory
        tank->lidarPoints[cast_num] = point;
    }
}

static void tankRenderLidar(Tank tank, b2HexColor color)
{
    // Rander tank lidar scan memory buffer 
    RGBf colorf = MakeRGBf(color);
    GLfloat radius = 0.01f;

    for (size_t cast_num = 0; cast_num < TANK_LIDAR_POINTS; cast_num++)
    {
        // Retrieve lidar point
        b2Vec2 point = tank.lidarPoints[cast_num];

         // Render lidar point
        GLfloat center[2] = {point.x / ARENA_WIDTH, point.y / ARENA_HEIGHT};
        renderCircle(center, radius, (float[]){colorf.r, colorf.g, colorf.b});
    }
}


/* Engine Methods */
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

    // Create boundaries
    b2BodyDef boundaryBodyDef = b2DefaultBodyDef();
    boundaryBodyDef.type = b2_staticBody;
    b2BodyId boundaryBodyId = b2CreateBody(worldId, &boundaryBodyDef);

    b2ShapeDef boundaryShapeDef = b2DefaultShapeDef();
    boundaryShapeDef.filter.categoryBits = WALL;

    b2Polygon boundaryPolygon;

    boundaryPolygon = b2MakeOffsetBox(0, ARENA_HEIGHT, (b2Vec2){-ARENA_WIDTH, 0}, 0); // left wall
    b2CreatePolygonShape(boundaryBodyId, &boundaryShapeDef, &boundaryPolygon);

    boundaryPolygon = b2MakeOffsetBox(0, ARENA_HEIGHT, (b2Vec2){ARENA_WIDTH, 0}, 0); // right wall
    b2CreatePolygonShape(boundaryBodyId, &boundaryShapeDef, &boundaryPolygon);

    boundaryPolygon = b2MakeOffsetBox(ARENA_WIDTH, 0, (b2Vec2){0, ARENA_HEIGHT}, 0); // top wall
    b2CreatePolygonShape(boundaryBodyId, &boundaryShapeDef, &boundaryPolygon);

    boundaryPolygon = b2MakeOffsetBox(ARENA_WIDTH, 0, (b2Vec2){0, -ARENA_HEIGHT}, 0); // bottom wall
    b2CreatePolygonShape(boundaryBodyId, &boundaryShapeDef, &boundaryPolygon);

    // Create tanks
    tank1 = tankCreateNew((b2Vec2){0.0f, 0.0f}, 0.0f, TANK1);    // TODO: pass in initial position and angle as arguments
    tank2 = tankCreateNew((b2Vec2){50.0f, 0.0f}, b2_pi/4, TANK2);
    
    initialized = true;

    return initialized;
}

void engineStep(TankAction tank1Action, TankAction tank2Action)
{
    // Step forward the physics engine
    if (!initialized)
        return;

    // Tank 1 controls
    tankRotateGun(tank1, tank1Action.gun_angle);

    tankForceTreads(tank1, tank1Action.tread_force[0], tank1Action.tread_force[1]); // TODO: clean up fn definition --> use vector as fn input, or 2x vars in TankAction struct

    if (tank1Action.fire_gun)
        tankFireGun(tank1);

    tankScanLidar(&tank1);  // TODO: fix scan location? here, the scan is always 1 step behind due to the physics engine step being called right after

    // Tank 2 controls
    tankRotateGun(tank2, tank2Action.gun_angle);

    tankForceTreads(tank2, tank2Action.tread_force[0], tank2Action.tread_force[1]);

    if (tank2Action.fire_gun)
        tankFireGun(tank2);

    tankScanLidar(&tank2);

    // Collision logic
    b2ContactEvents contactEvents = b2World_GetContactEvents(worldId);
    for (int count = 0; count < contactEvents.beginCount; count++)
    {
        // Retrieve contacting shape information
        b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + count;

        b2ShapeId shapeIdA = beginEvent->shapeIdA;
        uint32_t categoryBitsA = b2Shape_GetFilter(shapeIdA).categoryBits;

        b2ShapeId shapeIdB = beginEvent->shapeIdB;
        uint32_t categoryBitsB = b2Shape_GetFilter(shapeIdB).categoryBits;

        // Determine if a projectile was involved
        if (categoryBitsA != PROJECTILE && categoryBitsB != PROJECTILE)
        {
            continue;
        }

        // TODO: check if projectile is already removed?

        // Collision logic
        switch (categoryBitsA)
        {
            case PROJECTILE :
                // Remove projectile from world
                b2DestroyBody(b2Shape_GetBody(shapeIdA));
                break;

            case TANK1 :
                // Tank 1 has been hit by projectile
                printf("tank1 hit\n");
                break;

            case TANK2 :
                // Tank 2 has been hit by projectile
                printf("tank2 hit\n");
                break;

            default :
                // Something else has been hit by projectile
                printf("other hit\n");
                break;
        };

        switch (categoryBitsB)
        {
            case PROJECTILE :
                // Remove projectile from world
                b2DestroyBody(b2Shape_GetBody(shapeIdB));
                break;

            case TANK1 :
                // Tank 1 has been hit by projectile
                printf("tank1 hit\n");
                break;

            case TANK2 :
                // Tank 2 has been hit by projectile
                printf("tank2 hit\n");
                break;

            default :
                // Something else has been hit by projectile
                printf("other hit\n");
                break;
        };
    }

    // Step physics engine
    b2World_Step(worldId, TIME_STEP, SUB_STEPS);
}

void engineRender()
{
    // Render the physics engine
    if (!initialized)
        return;
    
    b2World_Draw(worldId, &debugDraw);

    tankRenderLidar(tank1, b2_colorMediumBlue); // TODO: put color choice in tank struct?
    tankRenderLidar(tank2, b2_colorOrange);
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
