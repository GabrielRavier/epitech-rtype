#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

struct Window
{
    std::shared_ptr<sf::RenderWindow> window;
};
