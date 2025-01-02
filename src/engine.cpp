// Tank Game (@kennedyengineering)

#include "engine.hpp"
#include "categories.hpp"

using namespace TankGame;

Engine::Engine()
{
    /* Create the engine */

    // Initialize box2d world
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 0.0f};
    mWorldId = b2CreateWorld(&worldDef);

    // Create boundaries
    b2BodyDef boundaryBodyDef = b2DefaultBodyDef();
    boundaryBodyDef.type = b2_staticBody;
    b2BodyId boundaryBodyId = b2CreateBody(mWorldId, &boundaryBodyDef);

    b2ShapeDef boundaryShapeDef = b2DefaultShapeDef();
    boundaryShapeDef.filter.categoryBits = CategoryBits::WALL;
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
