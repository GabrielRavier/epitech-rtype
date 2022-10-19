/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WindowManager
*/

#include "WindowManager.hpp"
#include <iostream>
#include "../../server/packets/packet_client_input.hpp"

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

bool WindowManager::ManageNetworkEvent(NetworkManager &manager)
{
    sf::Event            event;
    const std::bitset<8> last_inputs = _inputs;
    while (_window->pollEvent(event)) {
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::Key::Left)
                _inputs.set(InputType::LEFT, event.type == sf::Event::KeyPressed);
            else if (event.key.code == sf::Keyboard::Key::Right)
                _inputs.set(InputType::RIGHT, event.type == sf::Event::KeyPressed);
            else if (event.key.code == sf::Keyboard::Key::Up)
                _inputs.set(InputType::UP, event.type == sf::Event::KeyPressed);
            else if (event.key.code == sf::Keyboard::Key::Down)
                _inputs.set(InputType::DOWN, event.type == sf::Event::KeyPressed);
            else if (event.key.code == sf::Keyboard::Key::Space)
                _inputs.set(InputType::SHOOT, event.type == sf::Event::KeyPressed);
        } else if (event.type == sf::Event::Closed) {
            return (false);
        }
    }
    // Send to network.
    if (_inputs != last_inputs) {
        PacketClientInput packet(_inputs);

        manager.send(&packet);
    }
    return (true);
}

bool WindowManager::ManageEvent()
{
    sf::Event event;

    while (_window->pollEvent(event)) {
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::Key::Left)
                _inputs.set(InputType::LEFT, event.type == sf::Event::KeyPressed);
            else if (event.key.code == sf::Keyboard::Key::Right)
                _inputs.set(InputType::RIGHT, event.type == sf::Event::KeyPressed);
            else if (event.key.code == sf::Keyboard::Key::Up)
                _inputs.set(InputType::UP, event.type == sf::Event::KeyPressed);
            else if (event.key.code == sf::Keyboard::Key::Down)
                _inputs.set(InputType::DOWN, event.type == sf::Event::KeyPressed);
            else if (event.key.code == sf::Keyboard::Key::Space)
                _inputs.set(InputType::SHOOT, event.type == sf::Event::KeyPressed);
        } else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Button::Left)
                _clicked = event.type == sf::Event::MouseButtonPressed;
        } else if (event.type == sf::Event::Closed) {
            return (false);
        }
        _mouse = sf::Mouse::getPosition(*_window);
    }
    return (true);
}

std::bitset<8> WindowManager::GetInputs() const
{
    return (_inputs);
}

sf::Vector2i WindowManager::GetMousePosition() const
{
    return (_mouse);
}

bool WindowManager::MouseClicked() const
{
    return (_clicked);
}
