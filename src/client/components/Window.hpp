#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

struct Window
{
    std::shared_ptr<sf::RenderWindow> window;
};
