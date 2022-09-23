#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "../utils/Vec2.hpp"

struct Sprite
{
    sf::Texture texture;
    sf::Sprite sprite;
    Vec2 rectSize;
    Vec2 rectPos;
    int layer;
};