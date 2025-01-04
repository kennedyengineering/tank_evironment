// Tank Game (@kennedyengineering)

#include <iostream>

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
    /* Destroy the engine */

    // Destroy the physics engine
    b2DestroyWorld(mWorldId);
}

RegistryId Engine::addTank(const TankConfig& tankConfig)
{
    /* Add a tank */

    // Create a new tank in registry with id as argument
    return mTankRegistry.emplaceWithId(tankConfig, mWorldId);
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

    // Resolve resultant collisions
    handleCollisions();
}

void Engine::handleCollisions()
{
    /* Handle collisions */

    // Retrieve contact events
    b2ContactEvents contactEvents = b2World_GetContactEvents(mWorldId);

    // Define table
    struct TableEntry : b2ShapeId
    {
        bool operator<(const b2ShapeId& other) const
        {
            return std::tie(index1, world0, revision) < 
                   std::tie(other.index1, other.world0, other.revision);
        }
    };

    std::map<TableEntry, std::set<TableEntry>> table;

    // Populate table
    for (size_t contactNum = 0; contactNum < contactEvents.beginCount; contactNum++)
    {
        // Retrieve contact event
        b2ContactBeginTouchEvent* contactEvent = contactEvents.beginEvents + contactNum;

        // Retrieve shape ids
        b2ShapeId shapeIdA = contactEvent->shapeIdA;
        b2ShapeId shapeIdB = contactEvent->shapeIdB;

        // Retrieve category bits
        CategoryBits categoryBitsA = static_cast<CategoryBits>(b2Shape_GetFilter(shapeIdA).categoryBits);
        CategoryBits categoryBitsB = static_cast<CategoryBits>(b2Shape_GetFilter(shapeIdB).categoryBits);

        // Determine if a projectile was involved
        if (categoryBitsA != CategoryBits::PROJECTILE && categoryBitsB != CategoryBits::PROJECTILE)
        {
            continue;
        }

        // Handle the first shape being a projectile
        if (categoryBitsA == CategoryBits::PROJECTILE)
        {
            table[TableEntry{shapeIdA}].insert(TableEntry{shapeIdB});
        }

        // Handle the second shape being a projectile
        if (categoryBitsB == CategoryBits::PROJECTILE)
        {
            table[TableEntry{shapeIdB}].insert(TableEntry{shapeIdA});
        }
    }

    // Handle collision effects
    for (const std::pair<TableEntry, std::set<TableEntry>>& pair : table)
    {
        b2ShapeId projectileShapeId = pair.first;
        TankId sourceTankId = *static_cast<TankId*>(b2Shape_GetUserData(projectileShapeId));

        for (const b2ShapeId& contactShapeId : pair.second)
        {
            CategoryBits contactCategoryBits = static_cast<CategoryBits>(b2Shape_GetFilter(contactShapeId).categoryBits);

            switch (contactCategoryBits)
            {
                case CategoryBits::PROJECTILE:
                {
                    TankId otherTankId = *static_cast<TankId*>(b2Shape_GetUserData(contactShapeId));
                    std::cout << "projectile v projectile : " << sourceTankId << " hit " << otherTankId << std::endl;
                    break;
                }

                case CategoryBits::WALL:
                {
                    std::cout << "projectile v wall : " << sourceTankId << " hit wall" << std::endl;
                    break;
                }

                case CategoryBits::TANK:
                {
                    TankId otherTankId = *static_cast<TankId*>(b2Shape_GetUserData(contactShapeId));
                    std::cout << "projectile v tank : " << sourceTankId << " hit " << otherTankId << std::endl;
                    break;
                }

                default :
                {
                    std::cout << "projectile v unknown" << sourceTankId << " hit unknown" << std::endl;
                    break;
                }
            }
        }
    }
    
    // Destroy projectiles
    for (const std::pair<TableEntry, std::set<TableEntry>>& pair : table)
    {
        b2ShapeId projectileShapeId = pair.first;
        b2DestroyBody(b2Shape_GetBody(projectileShapeId));
    }
}
