#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

struct Text
{
    std::shared_ptr<sf::Text> text;
    std::shared_ptr<sf::Font> font;
};
