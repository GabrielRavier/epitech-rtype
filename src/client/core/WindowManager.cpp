/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WindowManager
*/

#include "WindowManager.hpp"

void WindowManager::Init(std::string title, int width, int height)
{
    this->_window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), title);
}

void WindowManager::Clear()
{
    this->_window->clear(sf::Color::Black);
}

void WindowManager::RenderSprite(std::shared_ptr<sf::Sprite> sprite, Vec2 position)
{
    sprite->setPosition(sf::Vector2f(position.x, position.y));
    this->_window->draw(*sprite);
}

void WindowManager::Update()
{
    this->_window->display();
}