// Tank Game (@kennedyengineering)

#include <algorithm>

#include "categories.hpp"
#include "tank.hpp"

using namespace TankGame;

// TODO: check for valid tankConfig values

Tank::Tank(TankId tankId, const TankConfig &tankConfig, b2WorldId worldId)
    : mTankId(tankId), mTankConfig(tankConfig), mWorldId(worldId) {
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
  bodyShapeDef.filter.categoryBits = CategoryBits::TANK_BODY;
  bodyShapeDef.userData = &mTankId;

  b2Polygon bodyPolygon =
      b2MakeBox(mTankConfig.bodyWidth / 2.0f, mTankConfig.bodyHeight / 2.0f);
  mTankShapeId = b2CreatePolygonShape(mTankBodyId, &bodyShapeDef, &bodyPolygon);

  // Construct the left tread shape
  b2ShapeDef leftTreadShapeDef = b2DefaultShapeDef();
  leftTreadShapeDef.filter.categoryBits = CategoryBits::TANK_BODY;
  leftTreadShapeDef.userData = &mTankId;

  b2Polygon leftTreadPolygon = b2MakeOffsetBox(
      mTankConfig.treadWidth / 2.0f, mTankConfig.treadHeight / 2.0f,
      (b2Vec2){mTankConfig.bodyWidth / 2.0f + mTankConfig.treadWidth / 2.0f, 0},
      0);
  mLeftTreadShapeId = b2CreatePolygonShape(mLeftTreadBodyId, &leftTreadShapeDef,
                                           &leftTreadPolygon);

  // Weld left tread to tank body
  b2WeldJointDef leftTreadJointDef = b2DefaultWeldJointDef();
  leftTreadJointDef.bodyIdA = mTankBodyId;
  leftTreadJointDef.bodyIdB = mLeftTreadBodyId;
  b2CreateWeldJoint(mWorldId, &leftTreadJointDef);

  // Construct the right tread shape
  b2ShapeDef rightTreadShapeDef = b2DefaultShapeDef();
  rightTreadShapeDef.filter.categoryBits = CategoryBits::TANK_BODY;
  rightTreadShapeDef.userData = &mTankId;

  b2Polygon rightTreadPolygon = b2MakeOffsetBox(
      mTankConfig.treadWidth / 2.0f, mTankConfig.treadHeight / 2.0f,
      (b2Vec2){-mTankConfig.bodyWidth / 2.0f - mTankConfig.treadWidth / 2.0f,
               0},
      0);
  mRightTreadShapeId = b2CreatePolygonShape(
      mRightTreadBodyId, &rightTreadShapeDef, &rightTreadPolygon);

  // Weld right tread to tank body
  b2WeldJointDef rightTreadJointDef = b2DefaultWeldJointDef();
  rightTreadJointDef.bodyIdA = mTankBodyId;
  rightTreadJointDef.bodyIdB = mRightTreadBodyId;
  b2CreateWeldJoint(mWorldId, &rightTreadJointDef);

  // Create the gun shape
  b2ShapeDef gunShapeDef = b2DefaultShapeDef();
  gunShapeDef.density = mTankConfig.gunDensity;
  gunShapeDef.filter.categoryBits = CategoryBits::TANK_GUN;
  gunShapeDef.filter.maskBits =
      CategoryBits::ALL & ~(CategoryBits::TANK_GUN | CategoryBits::TANK_BODY |
                            CategoryBits::PROJECTILE | CategoryBits::WALL);
  gunShapeDef.userData = &mTankId;

  b2Polygon gunPolygon =
      b2MakeOffsetBox(mTankConfig.gunWidth / 2.0f, mTankConfig.gunHeight / 2.0f,
                      (b2Vec2){0, mTankConfig.gunHeight / 2.0f}, 0);
  mGunShapeId = b2CreatePolygonShape(mGunBodyId, &gunShapeDef, &gunPolygon);

  // Create gun motor joint
  b2MotorJointDef jointDef = b2DefaultMotorJointDef();
  jointDef.bodyIdA = mTankBodyId;
  jointDef.bodyIdB = mGunBodyId;
  jointDef.maxForce = mTankConfig.gunMotorMaxForce;
  jointDef.maxTorque = mTankConfig.gunMotorMaxTorque;
  jointDef.correctionFactor = mTankConfig.gunMotorCorrectionFactor;

  mGunMotorJointId = b2CreateMotorJoint(mWorldId, &jointDef);

  // Reserve space in lidar data vector
  mLidarData.reserve(mTankConfig.lidarPoints);
}

Tank::~Tank() {
  /* Destroy the tank */

  // Check if world has been destroyed
  if (!b2World_IsValid(mWorldId)) {
    return;
  }

  // Destroy the tank bodies and joints
  b2DestroyBody(mTankBodyId);
  b2DestroyBody(mGunBodyId);
  b2DestroyBody(mLeftTreadBodyId);
  b2DestroyBody(mRightTreadBodyId);
}

void Tank::rotateGun(float angle) {
  /* Rotate the tank gun to a set angle (in radians) */

  // Enforce angle bounds
  angle = std::clamp(angle, mTankConfig.gunAngleMin, mTankConfig.gunAngleMax);

  // Update angular offset
  b2Joint_WakeBodies(mGunMotorJointId);
  b2MotorJoint_SetAngularOffset(mGunMotorJointId, angle);
}

b2ShapeId Tank::fireGun() {
  /* Fire the tank gun */

  // Create the projectile body
  b2BodyDef projectileBodyDef = b2DefaultBodyDef();
  projectileBodyDef.type = b2_dynamicBody;
  projectileBodyDef.position = b2Body_GetPosition(mGunBodyId);
  projectileBodyDef.rotation = b2Body_GetRotation(mGunBodyId);

  projectileBodyDef.linearVelocity =
      b2Body_GetWorldVector(mGunBodyId,
                            (b2Vec2){0, mTankConfig.projectileVelocity}) +
      b2Body_GetLinearVelocity(mTankBodyId);

  projectileBodyDef.isBullet = true;
  b2BodyId projectileBodyId = b2CreateBody(mWorldId, &projectileBodyDef);

  // Create the projectile shape
  b2ShapeDef projectileShapeDef = b2DefaultShapeDef();
  projectileShapeDef.filter.categoryBits = CategoryBits::PROJECTILE;
  projectileShapeDef.filter.maskBits =
      CategoryBits::ALL & ~CategoryBits::TANK_GUN;
  projectileShapeDef.userData = new TankId(mTankId);

  b2Polygon projectilePolygon = b2MakeOffsetBox(
      mTankConfig.gunWidth / 2.0f, mTankConfig.gunWidth / 2.0f,
      (b2Vec2){0, mTankConfig.gunHeight + mTankConfig.gunWidth / 2.0f}, 0);
  b2ShapeId projectileShapeId = b2CreatePolygonShape(
      projectileBodyId, &projectileShapeDef, &projectilePolygon);

  return projectileShapeId;
}

void Tank::moveLeftTread(float speed) {
  /* Move the left tread */

  // Enforce speed bounds
  speed =
      std::clamp(speed, -mTankConfig.treadMaxSpeed, mTankConfig.treadMaxSpeed);

  // Set speed of the left tread
  b2Vec2 leftTreadWorldVelocity =
      b2Body_GetWorldVector(mTankBodyId, (b2Vec2){0, speed});
  b2Body_SetLinearVelocity(mLeftTreadBodyId, leftTreadWorldVelocity);
}

void Tank::moveRightTread(float speed) {
  /* Move the right tread */

  // Enforce speed bounds
  speed =
      std::clamp(speed, -mTankConfig.treadMaxSpeed, mTankConfig.treadMaxSpeed);

  // Set speed of the right tread
  b2Vec2 rightTreadWorldVelocity =
      b2Body_GetWorldVector(mTankBodyId, (b2Vec2){0, speed});
  b2Body_SetLinearVelocity(mRightTreadBodyId, rightTreadWorldVelocity);
}

void Tank::scanLidar() {
  /* Perform a lidar scan using default range */

  scanLidar(mTankConfig.lidarRange);
}

void Tank::scanLidar(float range) {
  /* Perform a lidar scan using specified range
     Input: range of the lidar scan (meters)
     Note: If no objects are within ~range~ distance, that point will be ~range~
     distance away
  */

  // Compute angle resolution (radians per point)
  float angleResolution = 2 * b2_pi / mTankConfig.lidarPoints;

  // Find start position
  b2Vec2 startPosition = b2Body_GetPosition(mTankBodyId);

  // Construct query filter
  b2QueryFilter viewFilter = {.categoryBits = CategoryBits::ALL,
                              .maskBits = CategoryBits::ALL};

  // Define raycast context
  struct RayCastContext {
    b2Vec2 point;
    float fraction;
    TankId tankId;
  };

  // Define raycast callback
  auto rayCastCallback = [](b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal,
                            float fraction, void *context) -> float {
    // Retrieve context data
    RayCastContext *ctx = static_cast<RayCastContext *>(context);

    // Check if colliding with self
    TankId *otherTankIdPtr =
        static_cast<TankId *>(b2Shape_GetUserData(shapeId));
    if (otherTankIdPtr != nullptr) {
      TankId otherTankId = *otherTankIdPtr;
      CategoryBits otherCategoryBits =
          static_cast<CategoryBits>(b2Shape_GetFilter(shapeId).categoryBits);
      if (otherTankId == ctx->tankId &&
          (otherCategoryBits == CategoryBits::TANK_BODY ||
           otherCategoryBits == CategoryBits::TANK_GUN)) {
        // Continue and ignore this shape
        fraction = -1.0f;
      }
    }

    // Update context if not colliding with self
    if (fraction > 0.0f) {
      if (ctx->fraction > fraction) {
        ctx->fraction = fraction;
        ctx->point = point;
      }
    }

    return fraction;
  };

  // Clear vector
  mLidarData.clear();

  // Populate vector
  for (size_t pointNum = 0; pointNum < mTankConfig.lidarPoints; pointNum++) {
    // Compute angle
    float angle = angleResolution * pointNum;

    // Compute translation vector
    b2Rot rotation = b2MakeRot(angle);
    b2Vec2 endPosition = b2Body_GetWorldPoint(
        mTankBodyId, b2RotateVector(rotation, (b2Vec2){range, 0}));
    b2Vec2 translation = b2Sub(endPosition, startPosition);

    // Perform raycast
    RayCastContext context = {
        .point = endPosition, .fraction = 1.0f, .tankId = mTankId};
    b2World_CastRay(mWorldId, startPosition, translation, viewFilter,
                    rayCastCallback, &context);

    // Note: if you wanted to return a value of zero if no objects are within
    // range, check for a fraction of 1.0f here and set the context.point to
    // {0}.

    // Update vector
    mLidarData.push_back(context.point);
  }
}

std::vector<b2Vec2> Tank::getLidarData() {
  /* Get the lidar data vector */

  // Return data
  return mLidarData;
}

float Tank::getGunAngle() {
  /* Get the current angle of the gun */

  // Return angle (in radians)
  return b2RelativeAngle(b2Body_GetTransform(mGunBodyId).q,
                         b2Body_GetTransform(mTankBodyId).q);
}

b2Vec2 Tank::getPosition() {
  /* Get the center of the tank position */

  // Return position
  return b2Body_GetPosition(mTankBodyId);
}

b2Vec2 Tank::getVelocity() {
  /* Get the local velocity of the tank */

  // Return velocity
  return b2Body_GetLinearVelocity(mTankBodyId);
}

b2HexColor Tank::getProjectileColor() {
  /* Get the color of the projectile (for rendering) */

  // Return color
  return mTankConfig.projectileColor;
}

b2HexColor Tank::getLidarColor() {
  /* Get the color of the lidar point (for rendering) */

  // Return color
  return mTankConfig.lidarColor;
}

float Tank::getLidarRadius() {
  /* Get the radius of the lidar point (for rendering) */

  // Return radius
  return mTankConfig.lidarRadius;
}

std::vector<std::pair<b2ShapeId, b2HexColor>> Tank::getShapeIdsAndColors() {
  /* Get shapes of the tank (for rendering) */

  // Return shapes in order to be rendered
  return std::vector<std::pair<b2ShapeId, b2HexColor>>{
      std::pair<b2ShapeId, b2HexColor>{mTankShapeId, mTankConfig.tankColor},
      std::pair<b2ShapeId, b2HexColor>{mLeftTreadShapeId,
                                       mTankConfig.leftTreadColor},
      std::pair<b2ShapeId, b2HexColor>{mRightTreadShapeId,
                                       mTankConfig.rightTreadColor},
      std::pair<b2ShapeId, b2HexColor>{mGunShapeId, mTankConfig.gunColor}};
}
