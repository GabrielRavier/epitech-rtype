/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WeaponSystem
*/

#pragma once

#include <memory>
#include "../../client/core/System.hpp"
#include "../../client/core/Coordinator.hpp"
#include "../components/Transform.hpp"
#include "../components/Projectile.hpp"
#include "../components/Movement.hpp"

class WeaponSystem : public System
{
public:
    void Init();
    void Update() const;

private:
    static void createMissileProjectile(Weapon weapon, Transform transform);
    int         maxRateOfFire = 40;
    // std::vector<void (*)(Weapon)> createProjectile;
};
