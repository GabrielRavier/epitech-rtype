/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WindowManager
*/

#include "WindowManager.hpp"

void WindowManager::Init(std::string title, int width, int height)
{
    _window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), title);
}

void WindowManager::Clear()
{
    _window->clear(sf::Color::Black);
}

void WindowManager::RenderSprite(sf::Sprite sprite, Vec2 position)
{
    sprite.setPosition(sf::Vector2f(position.x, position.y));
    _window->draw(sprite);
}

void WindowManager::Update()
{
    _window->display();
}