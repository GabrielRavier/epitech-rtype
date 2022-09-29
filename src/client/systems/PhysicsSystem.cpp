#include "PhysicsSystem.hpp"
#include <iostream>

extern Coordinator gCoordinator;

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
}
