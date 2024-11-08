// Tank Game (@kennedyengineering)

#include "engine.h"
#include "render.h"

#include <box2d/box2d.h>
#include <stdio.h>

#define TIME_STEP   1.0f / 60.0f
#define SUB_STEPS   4.0f

#define ARENA_HEIGHT 48.8f   // football field (meters)
#define ARENA_WIDTH  109.7f

#define GROUND_FRICTION_COEF 0.3
#define GRAVITY_ACCEL 9.8

// TODO: refactor tank stuff into tank.h or something
#define TANK_BODY_HEIGHT 7.93f   // m1-abrams (meters)
#define TANK_BODY_WIDTH  3.66f
#define TANK_GUN_HEIGHT  5.805f
#define TANK_GUN_WIDTH   0.20f
#define TANK_TREAD_WIDTH 0.40f
// TODO: add tank constants as definitions

static bool initialized = false;

static b2WorldId worldId;
static b2DebugDraw debugDraw;

typedef struct Tank
{
    b2BodyId bodyId, gunId, leftTreadId, rightTreadId;
    b2ShapeId leftTreadShapeId, rightTreadShapeId;
    b2JointId motorId;
} Tank;

static Tank tank1, tank2;

static void RotateTankGun(Tank tank, float angle)
{
    // Move the tank gun to a specific angle relative to the tank body (in radians)
    b2MotorJoint_SetAngularOffset(tank.motorId, angle);
}

static void RotateTankBody(Tank tank, float angular_velocity)
{
    // Rotate tank to desired angular velocity
    // TODO: see how tank tread bodies affect rotational_inertia
    float rotational_inertia = b2Body_GetMassData(tank.bodyId).rotationalInertia + b2Body_GetMassData(tank.gunId).rotationalInertia;
    float current_angular_velocity = b2Body_GetAngularVelocity(tank.bodyId);
    float impulse = rotational_inertia * (angular_velocity - current_angular_velocity);

    b2Body_ApplyAngularImpulse(tank.bodyId, impulse, true);

    return;
}

static void ForceTankTreads(Tank tank, float force_left, float force_right)
{
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

static void MoveTankBody(Tank tank, b2Vec2 linear_velocity)
{
    // Move tank to desired linear velocity
    // TODO: see how tank tread bodies affect mass
    float mass = b2Body_GetMass(tank.bodyId) + b2Body_GetMass(tank.gunId);
    b2Vec2 current_linear_velocity = b2Body_GetLinearVelocity(tank.bodyId);
    b2Vec2 impulse = {
        mass * (linear_velocity.x - current_linear_velocity.x),
        mass * (linear_velocity.y - current_linear_velocity.y)
    };

    b2Body_ApplyLinearImpulseToCenter(tank.bodyId, impulse, true);

    return;
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
    tank.leftTreadId = b2CreateBody(worldId, &bodyDef);
    tank.rightTreadId = b2CreateBody(worldId, &bodyDef);

    // Create the body shape
    b2ShapeDef bodyShapeDef = b2DefaultShapeDef();
    bodyShapeDef.customColor = b2_colorGreenYellow;
    
    b2Polygon bodyPolygon = b2MakeBox(TANK_BODY_HEIGHT, TANK_BODY_WIDTH);
    b2CreatePolygonShape(tank.bodyId, &bodyShapeDef, &bodyPolygon);

    // Create the left tread shape
    b2ShapeDef leftTreadShapeDef = b2DefaultShapeDef();
    leftTreadShapeDef.customColor = b2_colorHotPink;

    b2Polygon leftTreadPolygon = b2MakeOffsetBox(TANK_BODY_HEIGHT, TANK_TREAD_WIDTH, (b2Vec2){0, TANK_BODY_HEIGHT/2.0f}, 0);
    tank.leftTreadShapeId = b2CreatePolygonShape(tank.leftTreadId, &leftTreadShapeDef, &leftTreadPolygon);

    b2WeldJointDef leftTreadJointDef = b2DefaultWeldJointDef();
    leftTreadJointDef.bodyIdA = tank.bodyId;
    leftTreadJointDef.bodyIdB = tank.leftTreadId;
    b2CreateWeldJoint(worldId, &leftTreadJointDef);

    // Create the right tread shape
    b2ShapeDef rightTreadShapeDef = b2DefaultShapeDef();
    rightTreadShapeDef.customColor = b2_colorHotPink;

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

    // RotateTankGun(tank1, 1.0f);
    // RotateTankGun(tank2, -1.0f);

    // MoveTankBody(tank1, (b2Vec2){2.1, 0.0});

    // RotateTankBody(tank1, 2.0f);

    // ForceTankTreads(tank1, 0.0f, 2000.0f);
    // ForceTankTreads(tank2, 2000.0f, 2000.0f);
    
    initialized = true;

    return initialized;
}

void engineStep(TankAction tank1Action, TankAction tank2Action)
{
    // Step forward the physics engine
    if (!initialized)
        return;

    // TODO: remove code duplication by creating a helper method

    RotateTankGun(tank1, tank1Action.gun_angle);
    RotateTankGun(tank2, tank2Action.gun_angle);

    switch (tank1Action.control_mode)
    {
        case MODE_FORCE_TREAD:
            ForceTankTreads(tank1, tank1Action.tread_force[0], tank1Action.tread_force[1]); // TODO: clean up fn definition --> use vector as fn input, or 2x vars in TankAction struct
            break;
        case MODE_LIN_ROT_VELOCITY:
            RotateTankBody(tank1, tank1Action.angular_velocity);
            MoveTankBody(tank1, (b2Vec2){tank1Action.linear_velocity[0], tank1Action.linear_velocity[1]});
            break;
    };

    switch (tank2Action.control_mode)
    {
        case MODE_FORCE_TREAD:
            ForceTankTreads(tank2, tank2Action.tread_force[0], tank2Action.tread_force[1]);
            break;
        case MODE_LIN_ROT_VELOCITY:
            RotateTankBody(tank2, tank2Action.angular_velocity);
            MoveTankBody(tank2, (b2Vec2){tank2Action.linear_velocity[0], tank2Action.linear_velocity[1]});
            break;
    }    

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
