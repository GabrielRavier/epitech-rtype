/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** MainMenuSystem
*/

#pragma once

#include "../core/System.hpp"
#include "../core/Coordinator.hpp"
#include "../components/Sprite.hpp"
#include "../components/Transform.hpp"

class MainMenuSystem : public System
{
public:
    void         Init();
    static SCENE Update(sf::Vector2i mousePosition, bool clicked);

private:
    Entity _singleplayerButton;
    Entity _multiplayerButton;
};
