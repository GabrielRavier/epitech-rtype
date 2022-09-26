#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

struct Sprite
{
    std::shared_ptr<sf::Texture> texture;
    std::shared_ptr<sf::Sprite>  sprite;
    sf::Vector2i                 rectSize;
    sf::Vector2i                 rectPos;
    int                          layer;
};
