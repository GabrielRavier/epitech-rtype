#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include "../utils/Vec2.hpp"

struct Sprite
{
    std::shared_ptr<sf::Texture> texture;
    std::shared_ptr<sf::Sprite>  sprite;
    Vec2                         rectSize;
    Vec2                         rectPos;
    int                          layer;
};
