/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** MainMenuSystem
*/

#include "MainMenuSystem.hpp"
#include <iostream>

extern Coordinator gCoordinator;

void MainMenuSystem::Init()
{
    const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
    texture->loadFromFile("./assets/singleplayer.png");
    sprite->setTexture(*texture, false);
    sprite->setScale(sf::Vector2f(1, 1));
    sprite->setTextureRect(sf::IntRect(0, 0, 176, 64));

    const std::shared_ptr<sf::Texture> mtexture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  msprite  = std::make_shared<sf::Sprite>();
    mtexture->loadFromFile("./assets/multiplayer.png");
    msprite->setTexture(*mtexture, false);
    msprite->setScale(sf::Vector2f(1, 1));
    msprite->setTextureRect(sf::IntRect(0, 0, 176, 64));

    _singleplayerButton = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(_singleplayerButton,
                              Sprite{texture, sprite, sf::Vector2i(176, 64), sf::Vector2i(0, 0), 1});
    gCoordinator.AddComponent(_singleplayerButton, Transform{sf::Vector2f(50, 50), sf::Vector2f(1, 1), 0});

    _multiplayerButton = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(_multiplayerButton,
                              Sprite{mtexture, msprite, sf::Vector2i(176, 64), sf::Vector2i(0, 0), 1});
    gCoordinator.AddComponent(_multiplayerButton, Transform{sf::Vector2f(50, 120), sf::Vector2f(1, 1), 0});
}

SCENE MainMenuSystem::Update(sf::Vector2i mousePosition, bool clicked)
{
    SCENE scene = SCENE::MAINMENU;
    if (clicked) {
        if (mousePosition.x > 50 && mousePosition.x < 226 && mousePosition.y > 50 && mousePosition.y < 114)
            scene = SCENE::SOLO;
        else if (mousePosition.x > 50 && mousePosition.x < 226 && mousePosition.y > 120 && mousePosition.y < 184)
            scene = SCENE::MULTIPLAYER;
    }
    return (scene);
}
