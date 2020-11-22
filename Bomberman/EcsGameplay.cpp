#include "EcsGameplay.hpp"
#include "MoveSystem.hpp"
#include "DrawSystem.hpp"
#include "GameplayStage.hpp"
#include "Transformable.hpp"
#include "Drawable.hpp"
#include "Movable.hpp"

EcsGameplay::EcsGameplay(const GameplayStage& stage)
    : gameplayStage{ stage }
{
    systems.add<MoveSystem>();
    systems.add<DrawSystem>();
    systems.configure();
    createPlayer();
}

bool EcsGameplay::update(const entityx::TimeDelta dt)
{
    systems.update<MoveSystem>(dt);
    return true;
}

void EcsGameplay::draw(sf::RenderWindow&)
{
    systems.update<DrawSystem>({});
}

void EcsGameplay::handleEvent(sf::Event&)
{
}

void EcsGameplay::createPlayer()
{
    auto entity = entities.create();
    entity.assign<Drawable>(gameplayStage.getTexture(ResourceID::BombermanFront));
    entity.assign<Transformable>(Transformable{ {64, 128}, { 300, 300} });
    entity.assign<Movable>(Movable{ { 64, 64 }, Direction::None });
}
