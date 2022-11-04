#include <stdexcept>
#include "NetworkManager.hpp"
#include "Coordinator.hpp"
#include "../systems/ObjectsSystem.hpp"
#include "../components/NetworkEntity.hpp"
#include "../components/Transform.hpp"
#include "../components/Sprite.hpp"
#include "../components/Player.hpp"

extern Coordinator                    gCoordinator;
extern std::shared_ptr<ObjectsSystem> gObjectsSystem;

void NetworkManager::processServerLogin(PacketServerLogin *packet)
{
    auto  entity        = gObjectsSystem->GetMe();
    auto &networkEntity = gCoordinator.GetComponent<NetworkEntity>(entity);

    // Set network ID.
    networkEntity.entityId = packet->entityId;
}

void NetworkManager::processServerKeepAlive(PacketServerKeepAlive *packet) {}

void NetworkManager::processServerEntityCreate(PacketServerEntityCreate *packet)
{
    if (packet->entityType == EntityType::PLAYER) {
        Entity entity = gCoordinator.CreateEntity();

        const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
        const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();

        texture->loadFromFile("./assets/players.gif");
        sprite->setTexture(*texture, false);
        sprite->setScale(sf::Vector2f(3, 3));
        sprite->setTextureRect(sf::IntRect(0, 0, 33, 17));

        gCoordinator.AddComponent<NetworkEntity>(entity, NetworkEntity{packet->entityId});
        gCoordinator.AddComponent<Sprite>(entity, Sprite{texture, sprite, sf::Vector2i(33, 17), sf::Vector2i(0, 0), 1});
        gCoordinator.AddComponent<Transform>(
            entity, Transform{sf::Vector2f(packet->posX, packet->posY), sf::Vector2f(3, 3), 0});

    } else if (packet->entityType == EntityType::BULLET) {
        Entity entity = gCoordinator.CreateEntity();

        const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
        const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
        const sf::Vector2f scale = sf::Vector2f(float(3 - (6 * static_cast<int>(packet->entityTeam))), 3.f);

        texture->loadFromFile("./assets/projectiles.gif");
        sprite->setTexture(*texture, false);
        sprite->setScale(scale);
        sprite->setTextureRect(sf::IntRect(248, 100, 16, 16));

        gCoordinator.AddComponent<NetworkEntity>(entity, NetworkEntity{packet->entityId});
        gCoordinator.AddComponent<Sprite>(entity, Sprite{texture, sprite, sf::Vector2i(50, 50), sf::Vector2i(250, 100), 1});
        gCoordinator.AddComponent<Transform>(entity, Transform{sf::Vector2f(packet->posX, packet->posY), scale, 0});

    } else if (packet->entityType == EntityType::MOB) {
        switch (packet->mobType) {
            case MobType::BLOP: {
                Entity entity = gCoordinator.CreateEntity();

                const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
                const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
                const sf::Vector2f                 scale   = sf::Vector2f(3, 3);

                texture->loadFromFile("./assets/blop.gif");
                sprite->setTexture(*texture, false);
                sprite->setScale(scale);
                sprite->setTextureRect(sf::IntRect(5, 34, 24, 34));

                gCoordinator.AddComponent<NetworkEntity>(entity, NetworkEntity{packet->entityId});
                gCoordinator.AddComponent<Sprite>(entity, Sprite{texture, sprite, sf::Vector2i(24, 34), sf::Vector2i(5, 34), 1});
                gCoordinator.AddComponent<Transform>(entity, Transform{sf::Vector2f(packet->posX, packet->posY), scale, 0});
                break;
            }

            case MobType::CROP: {
                Entity entity = gCoordinator.CreateEntity();

                const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
                const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
                const sf::Vector2f                 scale   = sf::Vector2f(3, 3);

                texture->loadFromFile("./assets/crop.gif");
                sprite->setTexture(*texture, false);
                sprite->setScale(scale);
                sprite->setTextureRect(sf::IntRect(1, 29, 29, 29));

                gCoordinator.AddComponent<NetworkEntity>(entity, NetworkEntity{packet->entityId});
                gCoordinator.AddComponent<Sprite>(entity, Sprite{texture, sprite, sf::Vector2i(29, 29), sf::Vector2i(1, 29), 1});
                gCoordinator.AddComponent<Transform>(entity, Transform{sf::Vector2f(packet->posX, packet->posY), scale, 0});
                break;
            }
        }

    } else {
        throw std::runtime_error("Invalid Entity Type.");
    }
}

void NetworkManager::processServerEntityDestroy(PacketServerEntityDestroy *packet)
{
    gCoordinator.DestroyEntity(gObjectsSystem->GetEntityByNetworkId(packet->entityId));
}

void NetworkManager::processServerUpdateHealth(PacketServerUpdateHealth *packet)
{
    gCoordinator.GetComponent<Player>(gObjectsSystem->GetMe()).life = packet->life;
}

void NetworkManager::processServerUpdatePos(PacketServerUpdatePos *packet)
{
    try {
        // Ignore self entity ID.
        if (gObjectsSystem->GetMe() == gObjectsSystem->GetEntityByNetworkId(packet->entityId))
            return;

        auto  entity    = gObjectsSystem->GetEntityByNetworkId(packet->entityId);
        auto &transform = gCoordinator.GetComponent<Transform>(entity);

        transform.position.x = static_cast<float>(packet->posX);
        transform.position.y = static_cast<float>(packet->posY);

    } catch (std::exception &e) {
        std::cerr << std::to_string(packet->entityId) << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

void NetworkManager::processServerUpdateScore(PacketServerUpdateScore *packet) {}
