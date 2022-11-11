/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** PhysicsSystem
*/

#pragma once

#include "../core/System.hpp"
#include "../core/Coordinator.hpp"
#include "../components/Movement.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Transform.hpp"
#include "../components/Enemy.hpp"
#include "../components/Projectile.hpp"
#include "../components/Player.hpp"

class PhysicsSystem : public System
{
public:
    void        Init(float windowWidth, float windowHeight);
    void        Update();
    void        ClientUpdate();
    static bool Collided(const Transform &transformA, const RigidBody &bodyA, const Transform &transformB,
                         const RigidBody &bodyB);

private:
    float _xLimit;
    float _yLimit;
};
