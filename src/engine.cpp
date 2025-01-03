// Tank Game (@kennedyengineering)

#include "engine.hpp"
#include "categories.hpp"

using namespace TankGame;

Engine::Engine(const Config& config)
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

    // Create tanks
    for (const TankConfig& tankConfig : config.tankConfigs)
    {
        Tank tank(tankConfig, mWorldId);
        mTanks.push_back(tank);
    }
}

Engine::~Engine()
{
    // Destroy the engine
}

void Engine::step()
{
    // Step the engine forward
}

void Engine::destroy()
{
    // Destroy the engine
}
