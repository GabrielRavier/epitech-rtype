/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** MovementSystem
*/

#include "MovementSystem.hpp"
#include "../core/NetworkServerManager.hpp"

extern Coordinator gCoordinator;
extern NetworkServerManager *gServerManager;

void MovementSystem::Init() {}

void MovementSystem::Update()
{
    for (auto const &entity : mEntities) {
        auto &move      = gCoordinator.GetComponent<Movement>(entity);
        auto &transform = gCoordinator.GetComponent<Transform>(entity);
        transform.posX += move.dirX * move.speed;
        transform.posY += move.dirY * move.speed;

        // Send entity position to peers.
        gServerManager->broadcast(new PacketServerUpdatePos(entity, transform.posX, transform.posY));
    }
}
