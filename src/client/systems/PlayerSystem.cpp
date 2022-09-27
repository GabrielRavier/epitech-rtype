/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** PlayerSystem
*/

#include "PlayerSystem.hpp"
#include <iostream>

extern Coordinator gCoordinator;

void PlayerSystem::Init()
{
    std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
    texture->loadFromFile("./assets/players.gif");
    sprite->setTexture(*texture, false);
    sprite->setScale(sf::Vector2f(3, 3));
    sprite->setTextureRect(sf::IntRect(0, 0, 33, 17));

    _player = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(_player, Sprite{texture, sprite, sf::Vector2i(33, 17), sf::Vector2i(0, 0), 0});
    gCoordinator.AddComponent(_player, Transform{sf::Vector2f(50, 50), sf::Vector2f(4, 4), 0});
    gCoordinator.AddComponent(_player, Player{"Player One", 10, 10});
}

void PlayerSystem::Update() {}
