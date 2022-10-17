#include <iostream>
#include <stdexcept>
#include "ObjectsSystem.hpp"
#include "../core/Coordinator.hpp"
#include "../components/NetworkEntity.hpp"

extern Coordinator gCoordinator;

void ObjectsSystem::Init(Entity me)
{
    _me = me;
}

Entity ObjectsSystem::GetEntityByNetworkId(Entity networkEntityId)
{
    for (const auto &entity : mEntities) {
        if (gCoordinator.GetComponent<NetworkEntity>(entity).entityId == networkEntityId) {
            return entity;
        }
    }

    throw std::runtime_error("Invalid Network Entity ID.");
}

Entity ObjectsSystem::GetMe()
{
    return _me;
}
