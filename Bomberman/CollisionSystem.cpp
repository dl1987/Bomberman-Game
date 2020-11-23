#include "CollisionSystem.hpp"
#include "Transformable.hpp"
#include "Player.hpp"


void CollisionSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta)
{
    handleMoveChangeEvents();

    es.each<Player, Collidable>([&](entityx::Entity playerEntity, Player&, Collidable& playerCollidable) {
        playerCollidable.direction = Direction::None;
        es.each<Collidable>([&](entityx::Entity otherEntity, Collidable& otherCollidable) {
            if (playerEntity != otherEntity)
            {
                const auto collisionInfo = checkCollision(playerEntity, otherEntity);
                if (collisionInfo && !shouldSkipCollision(playerEntity, otherCollidable))
                {
                    auto& playerTransformable = *playerEntity.component<Transformable>();
                    playerTransformable.position += collisionInfo->collisionRange;
                    playerCollidable.direction = collisionInfo->direction;
                }
                else if (!collisionInfo && shouldSkipCollision(playerEntity, otherCollidable))
                {
                    otherCollidable.spawner.reset();
                }
            }
        });
    });
}

void CollisionSystem::configure(entityx::EventManager& events)
{
    events.subscribe<MoveChangeEvent>(*this);
}

void CollisionSystem::receive(const MoveChangeEvent& event)
{
    moveChangeEvents.push_back(event);
}

void CollisionSystem::handleMoveChangeEvents()
{
    for (auto& event : moveChangeEvents)
    {
        if (event.entity.has_component<Collidable>())
        {
            auto collidable = event.entity.component<Collidable>();
            *collidable = Collidable{ collidable->spawner };
        }
    }
    moveChangeEvents.clear();
}

bool CollisionSystem::shouldSkipCollision(const entityx::Entity& playerEntity, Collidable& otherCollidable) const
{
    return playerEntity == otherCollidable.spawner;
}

std::optional<CollisionInfo> CollisionSystem::checkCollision(entityx::Entity playerEntity, entityx::Entity otherEntity) const
{
    const auto& playerTransformable = *playerEntity.component<Transformable>();
    const auto& otherTransformable = *otherEntity.component<Transformable>();

    const auto w = 0.5f * (playerTransformable.size.x + otherTransformable.size.x);
    const auto h = 0.5f * (playerTransformable.size.y + otherTransformable.size.y);
    const auto dx = (playerTransformable.position.x + 0.5f * playerTransformable.size.x) - (otherTransformable.position.x + 0.5f * otherTransformable.size.x);
    const auto dy = (playerTransformable.position.y + 0.5f * playerTransformable.size.y) - (otherTransformable.position.y + 0.5f * otherTransformable.size.y);

    if (abs(dx) <= w && abs(dy) <= h)
    {
        const auto wy = w * dy;
        const auto hx = h * dx;

        if (wy > hx)
        {
            if (wy > -hx)
                return CollisionInfo{ { 0, h - abs(dy) }, Direction::Up };
            else
                return CollisionInfo{ { -(w - abs(dx)), 0 }, Direction::Right };
        }
        else
        {
            if (wy > -hx)
                return CollisionInfo{ { w - abs(dx), 0 }, Direction::Left };
            else
                return CollisionInfo{ { 0, -(h - abs(dy)) }, Direction::Down };
        }
    }

    return std::nullopt;
}