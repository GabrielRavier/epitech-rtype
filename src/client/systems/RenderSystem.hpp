/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** RenderSystem
*/

#pragma once

#include <memory>
#include "../core/System.hpp"
#include "../core/Coordinator.hpp"
#include "../core/WindowManager.hpp"
#include "../components/Player.hpp"
#include "../components/Transform.hpp"
#include "../components/Sprite.hpp"

class RenderSystem : public System
{
public:
    void Init();
    void Update(const std::shared_ptr<WindowManager> &windowManager, bool online) const;
};
