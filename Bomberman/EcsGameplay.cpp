#include "EcsGameplay.hpp"
#include "MoveSystem.hpp"
#include "DrawSystem.hpp"
#include "AnimateSystem.hpp"
#include "CollisionSystem.hpp"
#include "ExplosionSystem.hpp"
#include "PowerUpSystem.hpp"
#include "GameplayStage.hpp"
#include "Transformable.hpp"
#include "Drawable.hpp"
#include "Movable.hpp"
#include "Animated.hpp"
#include "Player.hpp"
#include "Collidable.hpp"
#include "Constants.hpp"
#include "SpawnBombEvent.hpp"
#include "Map.hpp"
#include <algorithm>
#include <random>

EcsGameplay::EcsGameplay(const GameplayStage & stage)
    : gameplayStage{ stage }
{
    systems.add<MoveSystem>();
    systems.add<DrawSystem>(gameplayStage.getTextures());
    systems.add<AnimateSystem>();
    systems.add<CollisionSystem>();
    systems.add<ExplosionSystem>(gameplayStage.getTextures());
    systems.add<PowerUpSystem>(gameplayStage.getTextures());
    systems.configure();

    createPlayer();
    createMap();
}    

bool EcsGameplay::update(const entityx::TimeDelta dt)
{
    systems.update<MoveSystem>(dt);
    systems.update<ExplosionSystem>(dt);
    systems.update<AnimateSystem>(dt);
    systems.update<CollisionSystem>(dt);
    systems.update<PowerUpSystem>(dt);
    return true;
}

void EcsGameplay::draw(sf::RenderWindow&)
{
    systems.update<DrawSystem>({});
}

void EcsGameplay::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::EventType::KeyReleased
        && event.key.code == sf::Keyboard::Space)
    {
        auto player = entities.entities_with_components<Player>().begin();
        events.emit<SpawnBombEvent>({ *player });
    }
}

void EcsGameplay::createMap()
{
    Map map{};
    std::vector<sf::Vector2i> blankTilesIndexes;

    for (int i = 0; i < HEIGHT_TILES_NUM; i++)
    {
        for (int j = 0; j < WIDTH_TILES_NUM; j++)
        {
            auto tile = entities.create();
            tile.assign<Transformable>(Transformable{ { 64.f, 64.f }, { j * 64.f, i * 64.f } });

            if (i == 0 || i == 20 || j == 0 || j == 20 || i % 2 == 0 && j % 2 == 0)
            {
                tile.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::SolidBlock));
                tile.assign<Collidable>();
                tile.assign<Tile>(Tile{ TileType::SolidBlock });
                map.tiles[i][j] = { TileType::SolidBlock };
            }
            else
            {
                tile.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::BackgroundTile));
                tile.assign<Tile>(Tile{ TileType::None });
                map.tiles[i][j] = { TileType::None };
                blankTilesIndexes.emplace_back(sf::Vector2i{ i, j });
            }
        }
    }

    createExplodableBlocks(map, blankTilesIndexes);

    auto mapEntity = entities.create();
    mapEntity.assign<Map>(map);
}

void EcsGameplay::createExplodableBlocks(Map& map, std::vector<sf::Vector2i>& blankTilesIndexes)
{
    const auto& playerPosition = (*entities.entities_with_components<Player>().begin()).component<Transformable>()->position;

    auto rd = std::random_device{};
    auto rng = std::default_random_engine{ rd() };
    std::shuffle(std::begin(blankTilesIndexes), std::end(blankTilesIndexes), rng);

    auto playerPositionIndex = sf::Vector2i{ static_cast<int>(playerPosition.y / 64), static_cast<int>(playerPosition.x / 64) };
    auto splitPos = static_cast<uint32_t>(blankTilesIndexes.size() * (70 / 100.f));
    blankTilesIndexes.erase(std::begin(blankTilesIndexes) + splitPos, std::end(blankTilesIndexes));

    for (const auto& blankTileIndex : blankTilesIndexes)
    {
        if (std::abs(blankTileIndex.x - playerPositionIndex.x) >= 3 ||
            std::abs(blankTileIndex.y - playerPositionIndex.y) >= 3)
        {
            auto tile = entities.create();
            tile.assign<Transformable>(Transformable{ { 64.f, 64.f }, { blankTileIndex.y * 64.f, blankTileIndex.x * 64.f } });
            tile.assign<Collidable>();
            tile.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::ExplodableBlock));
            tile.assign<Tile>(Tile{ TileType::ExplodableBlock });
            map.tiles[blankTileIndex.x][blankTileIndex.y] = { TileType::ExplodableBlock };
        }
    }
}

void EcsGameplay::createPlayer()
{
    auto entity = entities.create();
    entity.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::BombermanFront));
    entity.assign<Transformable>(Transformable{ { 30, 30 }, { 81, 81 } });
    entity.assign<Movable>(Movable{ { 4*128, 4*128 }, Direction::None });
    entity.assign<Animated>(Animated{ 64, 128, 8, 1 });
    entity.assign<Collidable>();
    entity.assign<Player>();
    events.emit<MoveChangeEvent>({ entity, Direction::None });
}
