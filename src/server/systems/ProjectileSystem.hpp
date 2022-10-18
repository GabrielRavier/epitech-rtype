/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** ProjectileSystem
*/

#pragma once

#include <memory>
#include "../../client/core/System.hpp"
#include "../../client/core/Coordinator.hpp"
#include "../components/Transform.hpp"
#include "../components/Projectile.hpp"

class ProjectileSystem : public System
{
public:
    void Init();
    void Update() const;
};
