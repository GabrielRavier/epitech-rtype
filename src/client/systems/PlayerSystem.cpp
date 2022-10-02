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
    const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
    texture->loadFromFile("./assets/players.gif");
    sprite->setTexture(*texture, false);
    sprite->setScale(sf::Vector2f(3, 3));
    sprite->setTextureRect(sf::IntRect(0, 0, 33, 17));

    _player = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(_player, Sprite{texture, sprite, sf::Vector2i(33, 17), sf::Vector2i(0, 0), 0});
    gCoordinator.AddComponent(_player, Transform{sf::Vector2f(50, 50), sf::Vector2f(3, 3), 0});
    gCoordinator.AddComponent(_player, Player{"Player One", 10, 10});
    gCoordinator.AddComponent(_player, Movement{sf::Vector2f(0, 0), 5});
    gCoordinator.AddComponent(_player, RigidBody{sf::Vector2f(33, 17)});
}

void PlayerSystem::Update(std::unordered_map<sf::Keyboard::Key, bool> buttonsPressed) const
{
    auto &movement = gCoordinator.GetComponent<Movement>(_player);
    float x        = 0;
    float y        = 0;

    if (buttonsPressed[sf::Keyboard::Key::D])
        x += 1;
    if (buttonsPressed[sf::Keyboard::Key::Q])
        x -= 1;
    if (buttonsPressed[sf::Keyboard::Key::S])
        y += 1;
    if (buttonsPressed[sf::Keyboard::Key::Z])
        y -= 1;
    movement.movement.x = x;
    movement.movement.y = y;
}
