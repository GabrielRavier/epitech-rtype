/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** MovementSystem
*/

#include "MovementSystem.hpp"
#include "../core/NetworkManager.hpp"
#include "../systems/ObjectsSystem.hpp"
#include <iostream>

extern Coordinator gCoordinator;
extern NetworkManager *gNetworkManager;
extern std::shared_ptr<ObjectsSystem> gObjectsSystem;

void MovementSystem::Init() {}

void MovementSystem::Update()
{
    for (auto const &entity : mEntities) {
        auto &move      = gCoordinator.GetComponent<Movement>(entity);
        auto &transform = gCoordinator.GetComponent<Transform>(entity);
        transform.position.x += move.movement.x * move.speed;
        transform.position.y += move.movement.y * move.speed;
    }

    {
        auto entity     = gObjectsSystem->GetMe();
        auto &transform = gCoordinator.GetComponent<Transform>(entity);

        // Send position.
        gNetworkManager->send(new PacketClientPos(
            static_cast<uint16_t>(transform.position.x),
            static_cast<uint16_t>(transform.position.y)
        ));
    }
}
