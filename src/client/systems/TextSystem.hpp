/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** TextSystem
*/

#pragma once

#include <memory>
#include "../core/System.hpp"
#include "../core/Coordinator.hpp"
#include "../core/WindowManager.hpp"
#include "../components/Transform.hpp"
#include "../components/Text.hpp"

class TextSystem : public System
{
public:
    void Init();
    void Update(const std::shared_ptr<WindowManager> &windowManager) const;
};
