/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** LevelsSystem
*/

#pragma once

#include "../core/System.hpp"
#include "../core/Coordinator.hpp"
#include "../components/Sprite.hpp"
#include "../components/Transform.hpp"
#include "../components/Level.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <iostream>

class LevelsSystem : public System
{
public:
    void  Init();
    SCENE Update(sf::Vector2i mousePosition, bool clicked);
    void  createLevel(std::string name);
private:
    int _nbOfLevel = 0;
};