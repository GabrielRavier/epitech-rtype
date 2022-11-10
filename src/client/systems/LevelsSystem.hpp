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
#include "../components/Text.hpp"
#include "../components/Transform.hpp"
#include "../components/Level.hpp"
#include <filesystem>
#include <algorithm>
#include <string>
#include <fstream>

class LevelsSystem : public System
{
public:
    void        Init();
    SCENE       Update(sf::Vector2i mousePosition, bool clicked, std::string *pathLevel);
    void        createLevel(const std::string &name);
    static bool isLevelFinished(const std::string &path);

private:
    int _nbOfLevel = 0;
};
