#include "NetworkManager.hpp"
#include "Coordinator.hpp"
#include "../systems/ObjectsSystem.hpp"
#include "../components/NetworkEntity.hpp"
#include "../components/Transform.hpp"
#include "../components/Sprite.hpp"

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
        gCoordinator.AddComponent<Transform>(
            entity, Transform{sf::Vector2f(packet->posX, packet->posY), sf::Vector2f(3, 3), 0});
        gCoordinator.AddComponent<Sprite>(entity, Sprite{texture, sprite, sf::Vector2i(33, 17), sf::Vector2i(0, 0), 1});
    }
}

void NetworkManager::processServerEntityDestroy(PacketServerEntityDestroy *packet)
{
    gCoordinator.DestroyEntity(gObjectsSystem->GetEntityByNetworkId(packet->entityId));
}

void NetworkManager::processServerUpdateHealth(PacketServerUpdateHealth *packet) {}

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
