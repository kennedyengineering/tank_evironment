// Tank Game (@kennedyengineering)

#include <algorithm>
#include <iostream>

#include "categories.hpp"
#include "engine.hpp"

using namespace TankGame;

// TODO: check for valid config values
// TODO: add option for adding noise to lidar readings

Engine::Engine(const Config &config)
    : mConfig(config),
      mRenderEngine(std::ceil(config.arenaWidth * config.pixelDensity),
                    std::ceil(config.arenaHeight * config.pixelDensity)) {
  /* Create the engine */

  // Initialize box2d world
  b2WorldDef worldDef = b2DefaultWorldDef();
  worldDef.gravity = (b2Vec2){0.0f, 0.0f};
  mWorldId = b2CreateWorld(&worldDef);

  // Create arena boundaries
  b2BodyDef boundaryBodyDef = b2DefaultBodyDef();
  boundaryBodyDef.type = b2_staticBody;
  b2BodyId boundaryBodyId = b2CreateBody(mWorldId, &boundaryBodyDef);

  b2ShapeDef boundaryShapeDef = b2DefaultShapeDef();
  boundaryShapeDef.filter.categoryBits = CategoryBits::WALL;

  b2Polygon boundaryPolygon;

  boundaryPolygon = b2MakeOffsetBox(
      mConfig.arenaWallThickness / 2.0f, mConfig.arenaHeight / 2.0f,
      (b2Vec2){-mConfig.arenaWallThickness / 2.0f, mConfig.arenaHeight / 2.0f},
      0); // left wall
  b2CreatePolygonShape(boundaryBodyId, &boundaryShapeDef, &boundaryPolygon);

  boundaryPolygon = b2MakeOffsetBox(
      mConfig.arenaWallThickness / 2.0f, mConfig.arenaHeight / 2.0f,
      (b2Vec2){mConfig.arenaWidth + mConfig.arenaWallThickness / 2.0f,
               mConfig.arenaHeight / 2.0f},
      0); // right wall
  b2CreatePolygonShape(boundaryBodyId, &boundaryShapeDef, &boundaryPolygon);

  boundaryPolygon = b2MakeOffsetBox(
      mConfig.arenaWidth / 2.0f + mConfig.arenaWallThickness,
      mConfig.arenaWallThickness / 2.0f,
      (b2Vec2){mConfig.arenaWidth / 2.0f, -mConfig.arenaWallThickness / 2.0f},
      0); // top wall
  b2CreatePolygonShape(boundaryBodyId, &boundaryShapeDef, &boundaryPolygon);

  boundaryPolygon = b2MakeOffsetBox(
      mConfig.arenaWidth / 2.0f + mConfig.arenaWallThickness,
      mConfig.arenaWallThickness / 2.0f,
      (b2Vec2){mConfig.arenaWidth / 2.0f,
               mConfig.arenaHeight + mConfig.arenaWallThickness / 2.0f},
      0); // bottom wall
  b2CreatePolygonShape(boundaryBodyId, &boundaryShapeDef, &boundaryPolygon);
}

Engine::~Engine() {
  /* Destroy the engine */

  // Deallocate userData from projectile shape
  for (const b2ShapeId &projectileShapeId : mProjectileShapeIdVector) {
    delete static_cast<TankId *>(b2Shape_GetUserData(projectileShapeId));
  }

  // Destroy the physics engine
  b2DestroyWorld(mWorldId);
}

RegistryId Engine::addObstacle(const ObstacleConfig &obstacleConfig) {
  /* Add a obstacle */

  // Create a new obstacle in registry with id as argument
  return mObstacleRegistry.emplaceWithId(obstacleConfig, mWorldId);
}

void Engine::removeObstacle(RegistryId obstacleId) {
  /* Remove a obstacle */
  mObstacleRegistry.remove(obstacleId);
}

RegistryId Engine::addTank(const TankConfig &tankConfig) {
  /* Add a tank */

  // Create a new tank in registry with id as argument
  return mTankRegistry.emplaceWithId(tankConfig, mWorldId);
}

void Engine::removeTank(RegistryId tankId) {
  /* Remove a tank */
  mTankRegistry.remove(tankId);
}

void Engine::rotateTankGun(RegistryId tankId, float angle) {
  /* Rotate a tank gun */

  // Retrieve tank and apply method
  mTankRegistry.get(tankId).rotateGun(angle);
}

void Engine::fireTankGun(RegistryId tankId) {
  /* Fire a tank gun */

  // Retrieve tank and apply method, track projectile shapeId
  mProjectileShapeIdVector.push_back(mTankRegistry.get(tankId).fireGun());
}

void Engine::moveLeftTankTread(RegistryId tankId, float speed) {
  /* Move a tank left tread */

  // Retrieve tank and apply method
  mTankRegistry.get(tankId).moveLeftTread(speed);
}

void Engine::moveRightTankTread(RegistryId tankId, float speed) {
  /* Move a tank right tread */

  // Retrieve tank and apply method
  mTankRegistry.get(tankId).moveRightTread(speed);
}

std::vector<float> Engine::scanTankLidar(RegistryId tankId) {
  /* Scan a tank lidar, and get vector of distances */

  // Get the tank
  Tank &tank = mTankRegistry.get(tankId);

  // Get tank body position
  b2Vec2 position = tank.getPosition();

  // Perform a lidar scan
  tank.scanLidar();

  // Populate the vector
  std::vector<float> lidarData;

  for (const b2Vec2 &point : tank.getLidarData()) {
    lidarData.push_back(b2Distance(position, point));
  }

  // Return the vector
  return lidarData;
}

float Engine::getTankGunAngle(RegistryId tankId) {
  /* Get the current angle of a tank gun */

  // Return angle (in radians)
  return mTankRegistry.get(tankId).getGunAngle();
}

std::pair<float, float> Engine::getTankPosition(RegistryId tankId) {
  /* Get the current position of a tank */

  b2Vec2 position = mTankRegistry.get(tankId).getPosition();

  // Return position (in meters)
  return std::make_pair(position.x, position.y);
}

float Engine::getTankOrientation(RegistryId tankId) {
  /* Get the current world angle of a tank */

  // Return angle (in radians [-pi, pi])
  return mTankRegistry.get(tankId).getOrientation();
}

std::pair<float, float> Engine::getTankWorldVelocity(RegistryId tankId) {
  /* Get the current world linear velocity of a tank */

  b2Vec2 velocity = mTankRegistry.get(tankId).getWorldVelocity();

  // Return velocity (in meters per second)
  return std::make_pair(velocity.x, velocity.y);
}

std::pair<float, float> Engine::getTankLocalVelocity(RegistryId tankId) {
  /* Get the current local linear velocity of a tank */

  b2Vec2 velocity = mTankRegistry.get(tankId).getLocalVelocity();

  // Return velocity (in meters per second)
  return std::make_pair(velocity.x, velocity.y);
}

float Engine::getTankAngularVelocity(RegistryId tankId) {
  /* Get the current angular velocity of a tank */

  // Return angular velocity (in radians per second)
  return mTankRegistry.get(tankId).getAngularVelocity();
}

void Engine::clearImage() {
  /* Clear the image */

  // Clear the image
  mRenderEngine.clearImage(mConfig.clearColor);
}

void Engine::renderObstacle(RegistryId obstacleId) {
  /* Render all obstacles */

  // Render obstacle shapes returned in vector
  for (const std::pair<b2ShapeId, b2HexColor> &obstacleShapeIdAndColor :
       mObstacleRegistry.get(obstacleId).getShapeIdsAndColors()) {

    // Extract shapeId and color
    b2ShapeId obstacleShapeId = obstacleShapeIdAndColor.first;
    b2HexColor obstacleShapeColor = obstacleShapeIdAndColor.second;

    // Get shape
    b2Circle obstacleShapeCircle = b2Shape_GetCircle(obstacleShapeId);

    // Convert meters to pixels
    b2Vec2 convertedCenter =
        b2MulSV(mConfig.pixelDensity, obstacleShapeCircle.center);
    float convertedRadius = mConfig.pixelDensity * obstacleShapeCircle.radius;

    // Render the shape
    mRenderEngine.renderCircle(convertedCenter, convertedRadius,
                               obstacleShapeColor);
  }
}

void Engine::renderProjectiles() {
  /* Render all projectiles */

  // Render projectiles tracked in vector
  for (const b2ShapeId &projectileShapeId : mProjectileShapeIdVector) {
    // Get shape vertices and transform
    b2Polygon projectileShapePolygon = b2Shape_GetPolygon(projectileShapeId);
    b2Transform worldTransform =
        b2Body_GetTransform(b2Shape_GetBody(projectileShapeId));

    std::vector<b2Vec2> vertices;

    for (int i = 0; i < projectileShapePolygon.count; i++) {
      // Compute vertex location (meters)
      b2Vec2 transformedVertex =
          b2TransformPoint(worldTransform, projectileShapePolygon.vertices[i]);

      // Convert meters to pixels
      b2Vec2 convertedVertex = b2MulSV(mConfig.pixelDensity, transformedVertex);

      // Add to vector
      vertices.push_back(convertedVertex);
    }

    // Get source tank projectile color
    TankId sourceTankId =
        *static_cast<TankId *>(b2Shape_GetUserData(projectileShapeId));
    b2HexColor projectileColor =
        mTankRegistry.get(sourceTankId).getProjectileColor();

    // Render the projectile
    mRenderEngine.renderPolygon(vertices, projectileColor);
  }
}

void Engine::renderTank(RegistryId tankId) {
  /* Render a tank */

  // Render tank shapes returned in vector
  for (const std::pair<b2ShapeId, b2HexColor> &tankShapeIdAndColor :
       mTankRegistry.get(tankId).getShapeIdsAndColors()) {
    // Extract shapeId and color
    b2ShapeId tankShapeId = tankShapeIdAndColor.first;
    b2HexColor tankShapeColor = tankShapeIdAndColor.second;

    // Get shape vertices and transform
    b2Polygon tankShapePolygon = b2Shape_GetPolygon(tankShapeId);
    b2Transform worldTransform =
        b2Body_GetTransform(b2Shape_GetBody(tankShapeId));

    std::vector<b2Vec2> vertices;

    for (int i = 0; i < tankShapePolygon.count; i++) {
      // Compute vertex location (meters)
      b2Vec2 transformedVertex =
          b2TransformPoint(worldTransform, tankShapePolygon.vertices[i]);

      // Convert meters to pixels
      b2Vec2 convertedVertex = b2MulSV(mConfig.pixelDensity, transformedVertex);

      // Add to vector
      vertices.push_back(convertedVertex);
    }

    // Render the projectile
    mRenderEngine.renderPolygon(vertices, tankShapeColor);
  }
}

void Engine::renderTankLidar(RegistryId tankId) {
  /* Render a tank's latest lidar scan */

  // Get tank
  Tank &tank = mTankRegistry.get(tankId);

  // Get color
  b2HexColor color = tank.getLidarColor();

  // Get radius
  float radius = tank.getLidarRadius();

  // Get and render lidar data buffer
  for (const b2Vec2 &point : tank.getLidarData()) {
    // Convert meters to pixels
    b2Vec2 convertedPoint = b2MulSV(mConfig.pixelDensity, point);

    // Render the circle
    mRenderEngine.renderCircle(convertedPoint, radius, color);
  }
}

std::vector<unsigned char> Engine::getImageBuffer() {
  /* Return pixel buffer from render enging */

  // Return image
  return mRenderEngine.getBuffer();
}

std::pair<int, int> Engine::getImageDimensions() {
  /* Return dimensions of the image */

  // Return dimensions
  return mRenderEngine.getDimensions();
}

int Engine::getImageChannels() {
  /* Return number of channels of the image */

  // Return channels
  return mRenderEngine.getChannels();
}

void Engine::writeImageToPng(const std::filesystem::path &filePath) {
  /* Write the current image to disk as a .png */

  // Write .png to disk
  mRenderEngine.writeToPng(filePath);
}

std::vector<std::tuple<CategoryBits, RegistryId, RegistryId>> Engine::step() {
  /* Step the engine forward */

  // Step the physics engine
  b2World_Step(mWorldId, mConfig.timeStep, mConfig.subStep);

  // Resolve resultant collisions
  return handleCollisions();
}

std::vector<std::tuple<CategoryBits, RegistryId, RegistryId>>
Engine::handleCollisions() {
  /* Handle collisions
     Returns a vector of tuples <CategoryBits, srcTankId, hitTankId>
     If there is no collision with a tank, hitTankId is invalid
  */

  // Retrieve contact events
  b2ContactEvents contactEvents = b2World_GetContactEvents(mWorldId);

  // Define table
  struct TableEntry : b2ShapeId {
    bool operator<(const b2ShapeId &other) const {
      return std::tie(index1, world0, revision) <
             std::tie(other.index1, other.world0, other.revision);
    }
    bool operator==(const b2ShapeId &other) const {
      return std::tie(index1, world0, revision) ==
             std::tie(other.index1, other.world0, other.revision);
    }
  };

  std::map<TableEntry, std::set<TableEntry>> table;

  // Populate table
  for (size_t contactNum = 0; contactNum < contactEvents.beginCount;
       contactNum++) {
    // Retrieve contact event
    b2ContactBeginTouchEvent *contactEvent =
        contactEvents.beginEvents + contactNum;

    // Retrieve shape ids
    b2ShapeId shapeIdA = contactEvent->shapeIdA;
    b2ShapeId shapeIdB = contactEvent->shapeIdB;

    // Retrieve category bits
    CategoryBits categoryBitsA =
        static_cast<CategoryBits>(b2Shape_GetFilter(shapeIdA).categoryBits);
    CategoryBits categoryBitsB =
        static_cast<CategoryBits>(b2Shape_GetFilter(shapeIdB).categoryBits);

    // Determine if a projectile was involved
    if (categoryBitsA != CategoryBits::PROJECTILE &&
        categoryBitsB != CategoryBits::PROJECTILE) {
      continue;
    }

    // Handle the first shape being a projectile
    if (categoryBitsA == CategoryBits::PROJECTILE) {
      table[TableEntry{shapeIdA}].insert(TableEntry{shapeIdB});
    }

    // Handle the second shape being a projectile
    if (categoryBitsB == CategoryBits::PROJECTILE) {
      table[TableEntry{shapeIdB}].insert(TableEntry{shapeIdA});
    }
  }

  // Define output
  std::vector<std::tuple<CategoryBits, RegistryId, RegistryId>> output;

  // Handle collision effects
  for (const std::pair<TableEntry, std::set<TableEntry>> &pair : table) {
    b2ShapeId projectileShapeId = pair.first;
    TankId sourceTankId =
        *static_cast<TankId *>(b2Shape_GetUserData(projectileShapeId));

    for (const b2ShapeId &contactShapeId : pair.second) {
      CategoryBits contactCategoryBits = static_cast<CategoryBits>(
          b2Shape_GetFilter(contactShapeId).categoryBits);

      switch (contactCategoryBits) {
      case CategoryBits::OBSTACLE: {
        ObstacleId obstacleId =
            *static_cast<ObstacleId *>(b2Shape_GetUserData(contactShapeId));

        if (mConfig.verboseOutput) {
          std::cout << "projectile v obstacle : " << sourceTankId
                    << " hit obstacle " << obstacleId << std::endl;
        }

        output.push_back(
            std::make_tuple(contactCategoryBits, sourceTankId, obstacleId));

        break;
      }

      case CategoryBits::PROJECTILE: {
        TankId otherTankId =
            *static_cast<TankId *>(b2Shape_GetUserData(contactShapeId));

        if (mConfig.verboseOutput) {
          std::cout << "projectile v projectile : " << sourceTankId << " hit "
                    << otherTankId << std::endl;
        }

        output.push_back(
            std::make_tuple(contactCategoryBits, sourceTankId, otherTankId));

        break;
      }

      case CategoryBits::WALL: {
        if (mConfig.verboseOutput) {
          std::cout << "projectile v wall : " << sourceTankId << " hit wall"
                    << std::endl;
        }

        output.push_back(std::make_tuple(contactCategoryBits, sourceTankId, 0));

        break;
      }

      case CategoryBits::TANK_BODY: {
        TankId otherTankId =
            *static_cast<TankId *>(b2Shape_GetUserData(contactShapeId));

        if (mConfig.verboseOutput) {
          std::cout << "projectile v tank : " << sourceTankId << " hit "
                    << otherTankId << std::endl;
        }

        output.push_back(
            std::make_tuple(contactCategoryBits, sourceTankId, otherTankId));

        break;
      }

      default: {
        if (mConfig.verboseOutput) {
          std::cout << "projectile v unknown" << sourceTankId << " hit unknown"
                    << std::endl;
        }

        break;
      }
      }
    }
  }

  // Destroy projectiles
  for (const std::pair<TableEntry, std::set<TableEntry>> &pair : table) {
    b2ShapeId projectileShapeId = pair.first;

    // Deallocate userData from projectile shape
    delete static_cast<TankId *>(b2Shape_GetUserData(projectileShapeId));

    // Remove projectile shape from vector
    mProjectileShapeIdVector.erase(
        std::remove_if(
            mProjectileShapeIdVector.begin(), mProjectileShapeIdVector.end(),
            [pair](b2ShapeId shapeId) { return pair.first == shapeId; }),
        mProjectileShapeIdVector.end());

    b2DestroyBody(b2Shape_GetBody(projectileShapeId));
  }

  return output;
}
