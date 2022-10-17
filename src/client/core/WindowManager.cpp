/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WindowManager
*/

#include "WindowManager.hpp"
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

bool WindowManager::ManageEvent(NetworkManager &manager)
{
    sf::Event event;

    while (_window->pollEvent(event)) {
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
            std::bitset<8> last_inputs = _inputs;

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

            // Send to network.
            if (_inputs != last_inputs) {
                PacketClientInput packet(_inputs);

                manager.send(&packet);
            }

        } else if (event.type == sf::Event::Closed) {
            return (false);
        }
    }
    return (true);
}

std::bitset<8> WindowManager::GetInputs() const
{
    return (_inputs);
}
