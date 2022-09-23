/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** BackgroundSystem
*/

#pragma once

#include "../core/System.hpp"
#include "../core/Coordinator.hpp"
#include "../components/Sprite.hpp"
#include "../components/Transform.hpp"

class BackgroundSystem : public System
{
public:
    void Init();
    void Update(float dt);

private:
    Entity _firstBackground;
    Entity _secondBackground;
    float  _oldDt = 0;
};
