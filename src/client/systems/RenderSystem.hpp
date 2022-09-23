/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** RenderSystem
*/

#pragma once

#include "../core/System.hpp"
#include "../core/Coordinator.hpp"

class RenderSystem : public System
{
public:
    void Init();
    void Update(float dt);
};
