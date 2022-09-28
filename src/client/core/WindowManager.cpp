/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WindowManager
*/

#include "WindowManager.hpp"

void WindowManager::Init(const sf::String &title, int width, int height)
{
    this->_window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), title);
}

void WindowManager::Clear()
{
    this->_window->clear(sf::Color::Black);
}

void WindowManager::RenderSprite(const std::shared_ptr<sf::Sprite> &sprite, sf::Vector2f position)
{
    sprite->setPosition(position);
    this->_window->draw(*sprite);
}

void WindowManager::Update()
{
    this->_window->display();
}

bool WindowManager::ManageEvent()
{
    sf::Event event;

    while (_window->pollEvent(event)) {
        if (event.type == sf::Event::KeyPressed)
            _buttonsPressed[event.key.code] = true;
        else if (event.type == sf::Event::KeyReleased)
            _buttonsPressed[event.key.code] = false;
        else if (event.type == sf::Event::Closed)
            return (false);
    }
    return (true);
}

std::unordered_map<sf::Keyboard::Key, bool> WindowManager::GetPressedButtons() const
{
    return (_buttonsPressed);
}
