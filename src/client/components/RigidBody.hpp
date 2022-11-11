#pragma once

#include <SFML/Graphics.hpp>

struct RigidBody
{
    enum Type
    {
        PROJECTILE,
        ENEMY,
        PLAYER
    };

    sf::Vector2f    hitbox;
    RigidBody::Type type;
};
