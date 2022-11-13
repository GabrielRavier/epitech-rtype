#include "PhysicsSystem.hpp"
#include "ObjectsSystem.hpp"
#include "../core/NetworkServerManager.hpp"

extern Coordinator                    gCoordinator;
extern NetworkServerManager          *gServerManager;
extern std::shared_ptr<ObjectsSystem> gObjectsSystem;

void PhysicsSystem::Init() {}

void PhysicsSystem::Update() const
{
    std::unordered_set<Entity> toDelete;

    for (auto const &entity : mEntities) {
        auto &entityTransform = gCoordinator.GetComponent<Transform>(entity);
        auto &entityRigidBody = gCoordinator.GetComponent<RigidBody>(entity);

        for (auto const &target : mEntities) {
            auto &targetTransform = gCoordinator.GetComponent<Transform>(target);

            // Ignore same entity.
            if (entityTransform.type == targetTransform.type || targetTransform.type == EntityType::BULLET)
                continue;

            // Ignore if player is alread dead.
            if (entityTransform.type == EntityType::PLAYER && gCoordinator.GetComponent<Player>(entity).life == 0)
                continue;

            // Ignore if player is alread dead.
            if (targetTransform.type == EntityType::PLAYER && gCoordinator.GetComponent<Player>(target).life == 0)
                continue;

            auto &targetRigidBody = gCoordinator.GetComponent<RigidBody>(target);

            if (entityTransform.type == EntityType::BULLET) {
                auto &entityWeapon = gCoordinator.GetComponent<Projectile>(entity);

                // Skip same team mob.
                if (entityWeapon.team == Weapon::Team::ENEMY && targetTransform.type == EntityType::MOB)
                    continue;

                // Skip same team player.
                if (entityWeapon.team == Weapon::Team::PLAYERS && targetTransform.type == EntityType::PLAYER)
                    continue;

                // Detect collision.
                if (PhysicsSystem::Collided(entityTransform, entityRigidBody, targetTransform, targetRigidBody)) {
                    if (entityWeapon.team == Weapon::Team::ENEMY) {
                        toDelete.emplace(target);
                        toDelete.emplace(entity);
                    } else if (entityWeapon.team == Weapon::Team::PLAYERS) {
                        auto &targetEnemy = gCoordinator.GetComponent<Enemy>(target);
                        targetEnemy.life -= entityWeapon.damage;
                        if (targetEnemy.life <= 0)
                            toDelete.emplace(target);
                        toDelete.emplace(entity);
                    }
                }
            }
        }
    }

    for (auto const &entity : toDelete) {
        PacketServerEntityDestroy packetDestroy(entity);

        if (gCoordinator.GetComponent<Transform>(entity).type == EntityType::PLAYER) {
            PacketServerUpdateHealth packetUpdateHealth(entity, 0);

            // Set dead.
            gCoordinator.GetComponent<Player>(entity).life = 0;
            gCoordinator.GetComponent<Network>(entity).manager->send(&packetUpdateHealth);

            // Broadcast dead.
            gServerManager->broadcast(&packetDestroy);

        } else {

            gCoordinator.DestroyEntity(entity);
            gServerManager->broadcast(&packetDestroy);
        }
    }
}

bool PhysicsSystem::Collided(const Transform &transformA, const RigidBody &bodyA, const Transform &transformB,
                             const RigidBody &bodyB)
{
    return (transformA.posX < transformB.posX + bodyB.width && transformA.posX + bodyA.width > transformB.posX &&
            transformA.posY < transformB.posY + bodyB.height && bodyA.height + transformA.posY > transformB.posY);
}
