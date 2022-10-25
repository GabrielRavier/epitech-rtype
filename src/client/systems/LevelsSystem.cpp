/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** LevelsSystem
*/

#include "LevelsSystem.hpp"

extern Coordinator gCoordinator;

void LevelsSystem::Init()
{
    DIR* dirp = opendir("./levels");
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL)
        this->createLevel(std::string(dp->d_name));
    closedir(dirp);
}

SCENE LevelsSystem::Update(sf::Vector2i mousePosition, bool clicked)
{
    return (SCENE::LEVELSMENU);
}

void LevelsSystem::createLevel(std::string name)
{
    if (name == "." || name == "..")
        return;
    auto level = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(level, Level{"./levels/"+name});
    const std::shared_ptr<sf::Texture> mtexture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  msprite  = std::make_shared<sf::Sprite>();
    mtexture->loadFromFile("./assets/play_button.png");
    msprite->setTexture(*mtexture, false);
    msprite->setTextureRect(sf::IntRect(0, 0, 39, 39));
    gCoordinator.AddComponent(level, Sprite{mtexture, msprite, sf::Vector2i(39, 39), sf::Vector2i(0, 0), 1});
    gCoordinator.AddComponent(level, Transform{sf::Vector2f{float(100 * (_nbOfLevel % 5 + 1)), float(100 * (int(_nbOfLevel / 5) + 1))}, sf::Vector2f{1, 1}, 0});
    _nbOfLevel += 1;
}