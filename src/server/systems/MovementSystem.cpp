/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** MovementSystem
*/

#include "MovementSystem.hpp"
#include "../core/NetworkServerManager.hpp"

extern Coordinator           gCoordinator;
extern NetworkServerManager *gServerManager;

void MovementSystem::Init() {}

void MovementSystem::Update()
{
    std::unordered_set<Entity> toDelete;

    for (auto const &entity : mEntities) {
        auto &move      = gCoordinator.GetComponent<Movement>(entity);
        auto &transform = gCoordinator.GetComponent<Transform>(entity);

        // Set entity position.
        transform.posX += move.dirX * move.speed;
        transform.posY += move.dirY * move.speed;

        // Destroy entity.
        if (transform.posX < -100 || transform.posY < -100 ||
            (transform.type == EntityType::BULLET && transform.posX > 2000) || transform.posY > 700) {
            toDelete.emplace(entity);
            continue;
        }

        // Send entity position to peers.
        PacketServerUpdatePos sentPacket(entity, transform.posX, transform.posY);
        gServerManager->broadcast(&sentPacket);
    }

    for (auto const &entity : toDelete) {
        gCoordinator.DestroyEntity(entity);
        PacketServerEntityDestroy sentPacket(entity);
        gServerManager->broadcast(&sentPacket);
        std::cout << "DELETED ENTITY WITH ID " << std::to_string(entity) << std::endl;
    }
}
