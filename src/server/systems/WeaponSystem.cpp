/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WeaponSystem
*/

#include "WeaponSystem.hpp"
#include "../core/NetworkServerManager.hpp"

extern Coordinator           gCoordinator;
extern NetworkServerManager *gServerManager;

void WeaponSystem::Init() {}

void WeaponSystem::Update() const
{
    for (auto const &entity : mEntities) {
        auto       &weapon    = gCoordinator.GetComponent<Weapon>(entity);
        auto const &transform = gCoordinator.GetComponent<Transform>(entity);

        if (weapon.shootTimer > 0)
            weapon.shootTimer--;

        if (weapon.haveShot) {
            weapon.haveShot = false;

            if (weapon.shootTimer == 0) {
                weapon.shootTimer = 100 - weapon.rateOfFire;
                createMissileProjectile(weapon, transform);
            }
        }
    }
}

void WeaponSystem::createMissileProjectile(Weapon weapon, Transform transform)
{
    const Entity entity = gCoordinator.CreateEntity();

    gCoordinator.AddComponent<Projectile>(entity, Projectile{5, weapon.team});
    gCoordinator.AddComponent<Transform>(entity,
                                         Transform{EntityType::BULLET, MobType::NONE, transform.posX, transform.posY});
    gCoordinator.AddComponent<Movement>(entity, Movement{static_cast<int16_t>(1 - (2 * weapon.team)), 0, 30});
    gCoordinator.AddComponent<RigidBody>(entity, RigidBody{16 * 3, 8 * 3});

    // Broadcast bullet create.
    gServerManager->broadcast(new PacketServerEntityCreate(EntityType::BULLET, static_cast<EntityTeam>(weapon.team),
                                                           MobType::NONE, entity, transform.posX, transform.posY, 1));
}
