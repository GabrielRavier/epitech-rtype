/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** ProjectileSystem
*/

#include "ProjectileSystem.hpp"
#include "ObjectsSystem.hpp"
#include "../core/NetworkServerManager.hpp"

extern Coordinator                    gCoordinator;
extern NetworkServerManager          *gServerManager;
extern std::shared_ptr<ObjectsSystem> gObjectsSystem;

void ProjectileSystem::Init() {}

void ProjectileSystem::Update() const
{
    // std::unordered_set<Entity> toDelete;

    // for (auto const &entity : mEntities) {
    //     auto &entityTransform = gCoordinator.GetComponent<Transform>(entity);

    //     for (auto const &target : mEntities) {
    //         if (target == entity)
    //             continue;

    //         auto &targetTransform = gCoordinator.GetComponent<Transform>(entity);

    //         // Send entity position to peers.
    //         gServerManager->broadcast(new PacketServerUpdateHealth(entity, transform.posX, transform.posY));
    //     }
    // }

    // for (auto const &entity : toDelete) {
    //     gCoordinator.DestroyEntity(entity);
    //     gServerManager->broadcast(new PacketServerEntityDestroy(entity));
    // }
}
