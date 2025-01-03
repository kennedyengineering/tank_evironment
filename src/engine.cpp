// Tank Game (@kennedyengineering)

#include "engine.hpp"
#include "categories.hpp"

using namespace TankGame;

Engine::Engine(const Config& config) : mConfig(config)
{
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

    boundaryPolygon = b2MakeOffsetBox(0, config.arenaHeight, (b2Vec2){-(float)config.arenaWidth, 0}, 0); // left wall
    b2CreatePolygonShape(boundaryBodyId, &boundaryShapeDef, &boundaryPolygon);

    boundaryPolygon = b2MakeOffsetBox(0, config.arenaHeight, (b2Vec2){(float)config.arenaWidth, 0}, 0); // right wall
    b2CreatePolygonShape(boundaryBodyId, &boundaryShapeDef, &boundaryPolygon);

    boundaryPolygon = b2MakeOffsetBox(config.arenaWidth, 0, (b2Vec2){0, (float)config.arenaHeight}, 0); // top wall
    b2CreatePolygonShape(boundaryBodyId, &boundaryShapeDef, &boundaryPolygon);

    boundaryPolygon = b2MakeOffsetBox(config.arenaWidth, 0, (b2Vec2){0, -(float)config.arenaHeight}, 0); // bottom wall
    b2CreatePolygonShape(boundaryBodyId, &boundaryShapeDef, &boundaryPolygon);
}

Engine::~Engine()
{
    // Destroy the engine
}

RegistryId Engine::addTank(const TankConfig& tankConfig)
{
    /* Add a tank */
    // TODO: pass registryId to tank constructor
    return mTankRegistry.emplace(tankConfig, mWorldId);
}

// TODO: add tank control method, and action struct? or just retrieve tank and call methods directly?

void Engine::removeTank(RegistryId id)
{
    /* Remove a tank */
    mTankRegistry.remove(id);
}

void Engine::step()
{
    /* Step the engine forward */

    // Step the physics engine
    b2World_Step(mWorldId, mConfig.timeStep, mConfig.subStep);
}

void Engine::destroy()
{
    // Destroy the engine
}
