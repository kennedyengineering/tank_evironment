// Tank Game (@kennedyengineering)

#include "tank.hpp"

using namespace TankGame;

// TODO: check for valid tankConfig values
// TODO: make colors configurable in tankConfig

Tank::Tank(const TankConfig& tankConfig, b2WorldId worldId) : mWorldId(worldId)
{
    /* Create the tank */

    // Construct bodies
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){tankConfig.positionX, tankConfig.positionY};
    bodyDef.rotation = b2MakeRot(tankConfig.angle);

    mTankBodyId = b2CreateBody(mWorldId, &bodyDef);
    mGunBodyId = b2CreateBody(mWorldId, &bodyDef);
    mLeftTreadBodyId = b2CreateBody(mWorldId, &bodyDef);
    mRightTreadBodyId = b2CreateBody(mWorldId, &bodyDef);

    // Construct the tank body shape
    b2ShapeDef bodyShapeDef = b2DefaultShapeDef();
    bodyShapeDef.customColor = b2_colorGreenYellow;
    // bodyShapeDef.filter.categoryBits = categoryBits;
    
    b2Polygon bodyPolygon = b2MakeBox(tankConfig.bodyHeight, tankConfig.bodyWidth);
    b2CreatePolygonShape(mTankBodyId, &bodyShapeDef, &bodyPolygon);

    // Construct the left tread shape
    b2ShapeDef leftTreadShapeDef = b2DefaultShapeDef();
    leftTreadShapeDef.customColor = b2_colorHotPink;
    // leftTreadShapeDef.filter.categoryBits = categoryBits;

    b2Polygon leftTreadPolygon = b2MakeOffsetBox(tankConfig.bodyHeight, tankConfig.treadWidth, (b2Vec2){0, tankConfig.bodyHeight/2.0f}, 0);
    b2ShapeId leftTreadShapeId = b2CreatePolygonShape(mLeftTreadBodyId, &leftTreadShapeDef, &leftTreadPolygon);

    // Weld left tread to tank body
    b2WeldJointDef leftTreadJointDef = b2DefaultWeldJointDef();
    leftTreadJointDef.bodyIdA = mTankBodyId;
    leftTreadJointDef.bodyIdB = mLeftTreadBodyId;
    b2CreateWeldJoint(mWorldId, &leftTreadJointDef);

    // Construct the right tread shape
    b2ShapeDef rightTreadShapeDef = b2DefaultShapeDef();
    rightTreadShapeDef.customColor = b2_colorHotPink;
    // rightTreadShapeDef.filter.categoryBits = categoryBits;

    b2Polygon rightTreadPolygon = b2MakeOffsetBox(tankConfig.bodyHeight, tankConfig.treadWidth, (b2Vec2){0, -tankConfig.bodyHeight/2.0f}, 0);
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
    // gunShapeDef.filter.categoryBits = categoryBits;

    b2Polygon gunPolygon = b2MakeOffsetBox(tankConfig.gunHeight, tankConfig.gunWidth, (b2Vec2){tankConfig.gunHeight, 0}, 0);
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

}
