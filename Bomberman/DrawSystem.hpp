#pragma once

#include <entityx/entityx.h>
#include <SFML/Graphics/Texture.hpp>
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"
#include "MoveChangeEvent.hpp"

class DrawSystem : public entityx::System<DrawSystem>, public entityx::Receiver<DrawSystem>
{
public:
    DrawSystem(const ResourceHolder<sf::Texture, ResourceID>&);
    void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;
    void configure(entityx::EventManager&) override;
    void receive(const MoveChangeEvent&);

private:
    void handleMoveChangeEvents();

    const ResourceHolder<sf::Texture, ResourceID>& textures;
    std::vector<MoveChangeEvent> moveChangeEvents;
};

