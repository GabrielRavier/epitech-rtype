/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** PlayerSystem
*/

#pragma once

#include "../core/System.hpp"
#include "../core/Coordinator.hpp"
#include "../components/Player.hpp"
#include "../components/Movement.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Transform.hpp"
#include "../components/Sprite.hpp"

class PlayerSystem : public System
{
public:
    void Init();
    void Update();

private:
    Entity _player;
};
