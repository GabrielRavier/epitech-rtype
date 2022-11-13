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
        PacketClientPos packetClientPos(static_cast<int16_t>(transform.position.x),
                                        static_cast<int16_t>(transform.position.y));
        gNetworkManager->send(&packetClientPos);
    }
}

void PhysicsSystem::ClientUpdate()
{
    std::unordered_set<Entity> toDelete;

    for (auto const &entity : mEntities) {
        auto &entityTransform = gCoordinator.GetComponent<Transform>(entity);
        auto &entityRigidBody = gCoordinator.GetComponent<RigidBody>(entity);

        if (entityRigidBody.type == RigidBody::Type::PLAYER) {
            auto &movement = gCoordinator.GetComponent<Movement>(entity);
            if (entityTransform.position.x < 0)
                entityTransform.position.x += movement.speed;
            if (entityTransform.position.x + entityRigidBody.hitbox.x > _xLimit)
                entityTransform.position.x -= movement.speed;
            if (entityTransform.position.y < 0)
                entityTransform.position.y += movement.speed;
            if (entityTransform.position.y + entityRigidBody.hitbox.y > _yLimit)
                entityTransform.position.y -= movement.speed;
        }

        for (auto const &target : mEntities) {
            auto &targetRigidBody = gCoordinator.GetComponent<RigidBody>(target);

            // Ignore same entity.
            if (entityRigidBody.type == targetRigidBody.type || targetRigidBody.type == RigidBody::Type::PROJECTILE)
                continue;

            // Ignore if player is already dead.
            if (entityRigidBody.type == RigidBody::Type::PLAYER && gCoordinator.GetComponent<Player>(entity).life == 0)
                continue;

            // Ignore if player is already dead.
            if (targetRigidBody.type == RigidBody::Type::PLAYER && gCoordinator.GetComponent<Player>(target).life == 0)
                continue;

            auto &targetTransform = gCoordinator.GetComponent<Transform>(target);

            if (entityRigidBody.type == RigidBody::Type::PROJECTILE) {
                auto &entityProjectile = gCoordinator.GetComponent<Projectile>(entity);

                // Skip same team mob.
                if (entityProjectile.team == Weapon::Team::ENEMY && targetRigidBody.type == RigidBody::Type::ENEMY)
                    continue;

                // Skip same team player.
                if (entityProjectile.team == Weapon::Team::PLAYERS && targetRigidBody.type == RigidBody::Type::PLAYER)
                    continue;

                // Detect collision.
                if (PhysicsSystem::Collided(entityTransform, entityRigidBody, targetTransform, targetRigidBody)) {
                    if (entityProjectile.team == Weapon::Team::ENEMY) {
                        toDelete.emplace(target);
                        toDelete.emplace(entity);
                    } else if (entityProjectile.team == Weapon::Team::PLAYERS) {
                        auto &targetEnemy = gCoordinator.GetComponent<Enemy>(target);
                        targetEnemy.life -= entityProjectile.damage;
                        if (targetEnemy.life <= 0)
                            toDelete.emplace(target);
                        toDelete.emplace(entity);
                    }
                }
            }
        }
        if (entityTransform.position.x < -100)
            toDelete.emplace(entity);
        if (entityTransform.position.x > 2000 && entityRigidBody.type == RigidBody::Type::PROJECTILE)
            toDelete.emplace(entity);
    }

    for (auto const &entity : toDelete) {
        if (gCoordinator.GetComponent<RigidBody>(entity).type == RigidBody::Type::PLAYER)
            gCoordinator.GetComponent<Player>(entity).life = 0;
        else
            gCoordinator.DestroyEntity(entity);
    }
}

bool PhysicsSystem::Collided(const Transform &transformA, const RigidBody &bodyA, const Transform &transformB,
                             const RigidBody &bodyB)
{
    return (transformA.position.x < transformB.position.x + bodyB.hitbox.x &&
            transformA.position.x + bodyA.hitbox.x > transformB.position.x &&
            transformA.position.y < transformB.position.y + bodyB.hitbox.y &&
            transformA.position.y + bodyA.hitbox.y > transformB.position.y);
}
