/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** RenderSystem
*/

#pragma once

#include "../core/System.hpp"
#include "../core/Coordinator.hpp"
#include "../core/WindowManager.hpp"
#include "../components/Transform.hpp"
#include "../components/Sprite.hpp"
#include <memory>

class RenderSystem : public System
{
public:
    void Init();
    void Update(float dt, std::shared_ptr<WindowManager> windowManager);
};
