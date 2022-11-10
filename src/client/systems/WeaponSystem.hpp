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
#include "../components/Projectile.hpp"
#include "../components/Transform.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Movement.hpp"
#include "../components/Sprite.hpp"

class WeaponSystem : public System
{
public:
    void Init();
    void Update() const;

private:
    static void createMissileProjectile(Weapon weapon, Transform transform, RigidBody rigidbody);
};
