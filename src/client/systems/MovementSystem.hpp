/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** BackgroundSystem
*/

#pragma once

#include "../core/System.hpp"
#include "../core/Coordinator.hpp"
#include "../components/Movement.hpp"
#include "../components/Transform.hpp"

class MovementSystem : public System
{
public:
    void Init();
    void Update(float dt);
private:
    float _oldDt = 0;
};
