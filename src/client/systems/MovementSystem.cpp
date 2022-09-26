/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** MovementSystem
*/

#include "MovementSystem.hpp"
#include <iostream>

extern Coordinator gCoordinator;

void MovementSystem::Init()
{
}

void MovementSystem::Update(float dt)
{
    if (dt * 1000 + _oldDt > 2) {
        _oldDt = _oldDt + dt * 1000 - 2;
        for (auto const& entity : mEntities) {
            auto& move = gCoordinator.GetComponent<Movement>(entity);
            auto& transform = gCoordinator.GetComponent<Transform>(entity);
            transform.position.x += move.movement.x * move.speed;
            transform.position.y += move.movement.y * move.speed;
        }
    } else
        _oldDt += dt * 1000;
}