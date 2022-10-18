/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WeaponSystem
*/

#include "WeaponSystem.hpp"
#include "../core/NetworkServerManager.hpp"

extern Coordinator gCoordinator;
extern NetworkServerManager *gServerManager;

void WeaponSystem::Init() {}

void WeaponSystem::Update() const
{
    for (auto const &entity : mEntities) {
        auto       &weapon    = gCoordinator.GetComponent<Weapon>(entity);
        auto const &transform = gCoordinator.GetComponent<Transform>(entity);

        if (weapon.haveShot) {
            weapon.haveShot = false;

            if (weapon.shootTimer > 0) {
                weapon.shootTimer--;

            } else {
                weapon.shootTimer = maxRateOfFire - weapon.rateOfFire;
                createMissileProjectile(weapon, transform);
            }
        }
    }
}

void WeaponSystem::createMissileProjectile(Weapon weapon, Transform transform)
{
    Entity entity = gCoordinator.CreateEntity();

    gCoordinator.AddComponent<Projectile>(entity, Projectile { 10, weapon.team });
    gCoordinator.AddComponent<Transform>(entity, Transform { EntityType::BULLET, transform.posX, transform.posY });
    gCoordinator.AddComponent<Movement>(entity, Movement { static_cast<int>(1 - (2 * weapon.team)), 0, 40});

    // Broadcast bullet create.
    gServerManager->broadcast(new PacketServerEntityCreate(EntityType::BULLET, static_cast<EntityTeam>(weapon.team), entity, transform.posX, transform.posY, 1));
}
