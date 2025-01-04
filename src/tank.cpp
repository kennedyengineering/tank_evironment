// Tank Game (@kennedyengineering)

#include "tank.hpp"
#include "categories.hpp"

using namespace TankGame;

// TODO: check for valid tankConfig values
// TODO: make colors configurable in tankConfig

Tank::Tank(TankId tankId, const TankConfig& tankConfig, b2WorldId worldId) : mTankId(tankId), mTankConfig(tankConfig), mWorldId(worldId)
{
    /* Create the tank */

    // Construct bodies
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){mTankConfig.positionX, mTankConfig.positionY};
    bodyDef.rotation = b2MakeRot(mTankConfig.angle);

    mTankBodyId = b2CreateBody(mWorldId, &bodyDef);
    mGunBodyId = b2CreateBody(mWorldId, &bodyDef);
    mLeftTreadBodyId = b2CreateBody(mWorldId, &bodyDef);
    mRightTreadBodyId = b2CreateBody(mWorldId, &bodyDef);

    // Construct the tank body shape
    b2ShapeDef bodyShapeDef = b2DefaultShapeDef();
    bodyShapeDef.customColor = b2_colorGreenYellow;
    bodyShapeDef.filter.categoryBits = CategoryBits::TANK;
    bodyShapeDef.userData = &mTankId;
    
    b2Polygon bodyPolygon = b2MakeBox(mTankConfig.bodyHeight, mTankConfig.bodyWidth);
    b2CreatePolygonShape(mTankBodyId, &bodyShapeDef, &bodyPolygon);

    // Construct the left tread shape
    b2ShapeDef leftTreadShapeDef = b2DefaultShapeDef();
    leftTreadShapeDef.customColor = b2_colorHotPink;
    leftTreadShapeDef.filter.categoryBits = CategoryBits::TANK;
    leftTreadShapeDef.userData = &mTankId;

    b2Polygon leftTreadPolygon = b2MakeOffsetBox(mTankConfig.bodyHeight, mTankConfig.treadWidth, (b2Vec2){0, mTankConfig.bodyHeight/2.0f}, 0);
    b2ShapeId leftTreadShapeId = b2CreatePolygonShape(mLeftTreadBodyId, &leftTreadShapeDef, &leftTreadPolygon);

    // Weld left tread to tank body
    b2WeldJointDef leftTreadJointDef = b2DefaultWeldJointDef();
    leftTreadJointDef.bodyIdA = mTankBodyId;
    leftTreadJointDef.bodyIdB = mLeftTreadBodyId;
    b2CreateWeldJoint(mWorldId, &leftTreadJointDef);

    // Construct the right tread shape
    b2ShapeDef rightTreadShapeDef = b2DefaultShapeDef();
    rightTreadShapeDef.customColor = b2_colorHotPink;
    rightTreadShapeDef.filter.categoryBits = CategoryBits::TANK;
    rightTreadShapeDef.userData = &mTankId;

    b2Polygon rightTreadPolygon = b2MakeOffsetBox(mTankConfig.bodyHeight, mTankConfig.treadWidth, (b2Vec2){0, -mTankConfig.bodyHeight/2.0f}, 0);
    b2ShapeId rightTreadShapeId = b2CreatePolygonShape(mRightTreadBodyId, &rightTreadShapeDef, &rightTreadPolygon);

    // Weld right tread to tank body
    b2WeldJointDef rightTreadJointDef = b2DefaultWeldJointDef();
    rightTreadJointDef.bodyIdA = mTankBodyId;
    rightTreadJointDef.bodyIdB = mRightTreadBodyId;
    b2CreateWeldJoint(mWorldId, &rightTreadJointDef);

    // Create the gun shape
    b2ShapeDef gunShapeDef = b2DefaultShapeDef();
    gunShapeDef.density = 0.001f;                       // TODO: still need this?
    gunShapeDef.customColor = b2_colorGreen;
    gunShapeDef.filter.categoryBits = CategoryBits::TANK;
    gunShapeDef.userData = &mTankId;

    b2Polygon gunPolygon = b2MakeOffsetBox(mTankConfig.gunHeight, mTankConfig.gunWidth, (b2Vec2){mTankConfig.gunHeight, 0}, 0);
    b2CreatePolygonShape(mGunBodyId, &gunShapeDef, &gunPolygon);

    // Create gun motor joint
    b2MotorJointDef jointDef = b2DefaultMotorJointDef();
    jointDef.bodyIdA = mTankBodyId;
    jointDef.bodyIdB = mGunBodyId;
    jointDef.maxForce = 5.0f;       // TODO: make configurable?
    jointDef.maxTorque = 20.0f;
    jointDef.correctionFactor = 0.05;

    mGunMotorJointId = b2CreateMotorJoint(mWorldId, &jointDef);
}

Tank::~Tank()
{
    /* Destroy the tank */

    // Check if world has been destroyed
    if (!b2World_IsValid(mWorldId))
    {
        return;
    }

    // Destroy the tank bodies and joints
    b2DestroyBody(mTankBodyId);
    b2DestroyBody(mGunBodyId);
    b2DestroyBody(mLeftTreadBodyId);
    b2DestroyBody(mRightTreadBodyId);
}

void Tank::rotateGun(float angle)
{
    /* Rotate the tank gun to a set angle (in radians) */

    b2MotorJoint_SetAngularOffset(mGunMotorJointId, angle);
}

void Tank::fireGun()
{
    /* Fire the tank gun */

    // Create the projectile body
    b2BodyDef projectileBodyDef = b2DefaultBodyDef();
    projectileBodyDef.type = b2_dynamicBody;
    projectileBodyDef.position = b2Body_GetPosition(mGunBodyId);
    projectileBodyDef.rotation = b2Body_GetRotation(mGunBodyId);
    projectileBodyDef.linearVelocity = b2Body_GetWorldVector(mGunBodyId, (b2Vec2){mTankConfig.projectileVelocity, 0.0f});   // TODO: add gun's current velocity
    projectileBodyDef.isBullet = true;
    b2BodyId projectileBodyId = b2CreateBody(mWorldId, &projectileBodyDef);

    // Create the projectile shape
    b2ShapeDef projectileShapeDef = b2DefaultShapeDef();
    projectileShapeDef.customColor = b2_colorGray;
    projectileShapeDef.filter.categoryBits = CategoryBits::PROJECTILE;
    projectileShapeDef.filter.maskBits = CategoryBits::ALL;
    projectileShapeDef.userData = new TankId(mTankId);

    b2Polygon projectilePolygon = b2MakeOffsetBox(mTankConfig.gunWidth, mTankConfig.gunWidth, (b2Vec2){mTankConfig.gunHeight*2+mTankConfig.gunWidth, 0}, 0);
    b2CreatePolygonShape(projectileBodyId, &projectileShapeDef, &projectilePolygon);
}

void Tank::moveLeftTread(float force)
{
    /* Move the left tread */

    // Apply force to the left tread
    b2Vec2 leftTreadWorldForce = b2Body_GetWorldVector(mTankBodyId, (b2Vec2){force, 0});
    b2Body_ApplyForceToCenter(mLeftTreadBodyId, leftTreadWorldForce, true);

    // TODO: compute friction forces, or other motion model stuff (set velocity?)
}

void Tank::moveRightTread(float force)
{
    /* Move the right tread */

    // Apply force to the right tread
    b2Vec2 rightTreadWorldForce = b2Body_GetWorldVector(mTankBodyId, (b2Vec2){force, 0});
    b2Body_ApplyForceToCenter(mRightTreadBodyId, rightTreadWorldForce, true);

    // TODO: compute friction forces, or other motion model stuff (set velocity?)
}

std::vector<b2Vec2> Tank::scanLidar(float range)
{
    /* Perform a lidar scan */

    // TODO: see if range returns endPosition or zero (want to return endPosition...)

    // Compute angle resolution (radians per point)
    float angleResolution = 2*b2_pi/mTankConfig.lidarPoints;

    // Find start position
    b2Vec2 startPosition = b2Body_GetPosition(mTankBodyId);

    // Construct query filter
    b2QueryFilter viewFilter = {.categoryBits=CategoryBits::ALL, .maskBits=CategoryBits::ALL};

    // Define raycast context
    struct RayCastContext
    {
        b2Vec2 point;
        TankId tankId;
    };

    // Define raycast callback
    auto rayCastCallback = [](b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context) -> float {
        // Retrieve context data
        RayCastContext* ctx = static_cast<RayCastContext*>(context);
        ctx->point = point;

        // Check if colliding with self
        TankId otherTankId = *static_cast<TankId*>(b2Shape_GetUserData(shapeId));
        if (otherTankId == ctx->tankId)
        {
            // Continue and ignore this shape
            fraction = -1;
        }

        return fraction;
    };

    // Define vector
    std::vector<b2Vec2> points(mTankConfig.lidarPoints);

    // Populate vector
    for (size_t pointNum = 0; pointNum < mTankConfig.lidarPoints; pointNum++)
    {
        // Compute angle
        float angle = angleResolution*pointNum;

        // Compute translation vector
        b2Rot rotation = b2MakeRot(angle);
        b2Vec2 endPosition = b2Body_GetWorldPoint(mTankBodyId, b2RotateVector(rotation, (b2Vec2){range, 0}));
        b2Vec2 translation = b2Sub(endPosition, startPosition);

        // Perform raycast
        RayCastContext context = {.point = (b2Vec2){0}, .tankId = mTankId};
        b2World_CastRay(mWorldId, startPosition, translation, viewFilter, rayCastCallback, &context);

        // Update vector
        points[pointNum] = context.point;
    }

    return points;
}
