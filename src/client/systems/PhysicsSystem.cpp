/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** PhysicsSystem
*/

#include "PhysicsSystem.hpp"
#include "../core/NetworkManager.hpp"
#include "../systems/ObjectsSystem.hpp"

extern Coordinator                    gCoordinator;
extern NetworkManager                *gNetworkManager;
extern std::shared_ptr<ObjectsSystem> gObjectsSystem;

void PhysicsSystem::Init(float windowWidth, float windowHeight)
{
    _xLimit = windowWidth;
    _yLimit = windowHeight;
}

void PhysicsSystem::Update()
{
    for (auto const &entity : mEntities) {
        auto              &transform = gCoordinator.GetComponent<Transform>(entity);
        auto const        &rigidbody = gCoordinator.GetComponent<RigidBody>(entity);
        auto              &movement  = gCoordinator.GetComponent<Movement>(entity);
        const sf::Vector2f hitbox =
            sf::Vector2f(rigidbody.hitbox.x * transform.scale.x, rigidbody.hitbox.y * transform.scale.y);
        if (transform.position.x < 0)
            transform.position.x += movement.speed;
        if (transform.position.x + hitbox.x > _xLimit)
            transform.position.x -= movement.speed;
        if (transform.position.y < 0)
            transform.position.y += movement.speed;
        if (transform.position.y + hitbox.y > _yLimit)
            transform.position.y -= movement.speed;
    }

    {
        auto  entity    = gObjectsSystem->GetMe();
        auto &transform = gCoordinator.GetComponent<Transform>(entity);

        // Send position.
        gNetworkManager->send(new PacketClientPos(static_cast<int16_t>(transform.position.x),
                                                  static_cast<int16_t>(transform.position.y)));
    }
}
