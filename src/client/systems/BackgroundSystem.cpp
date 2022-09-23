/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** BackgroundSystem
*/

#include "BackgroundSystem.hpp"

extern Coordinator gCoordinator;

void BackgroundSystem::Init()
{
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("./assets/background.png");
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture, false);

    _firstBackground = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(_firstBackground, Sprite{.texture  = backgroundTexture,
                                                       .sprite   = backgroundSprite,
                                                       .rectSize = Vec2(960, 540),
                                                       .rectPos  = Vec2(0, 0),
                                                       .layer    = 0});
    gCoordinator.AddComponent(_firstBackground, Transform{.position = Vec2(0, 0), .scale = Vec2(1, 1), .rotation = 0});

    _secondBackground = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(_secondBackground, Sprite{.texture  = backgroundTexture,
                                                        .sprite   = backgroundSprite,
                                                        .rectSize = Vec2(960, 540),
                                                        .rectPos  = Vec2(0, 0),
                                                        .layer    = 0});
    gCoordinator.AddComponent(_secondBackground,
                              Transform{.position = Vec2(960, 0), .scale = Vec2(1, 1), .rotation = 0});
}

void BackgroundSystem::Update(float dt)
{
    if (dt + _oldDt > 100.f) {
        _oldDt        = _oldDt + dt - 100.f;
        auto &firstB  = gCoordinator.GetComponent<Transform>(_firstBackground);
        auto &secondB = gCoordinator.GetComponent<Transform>(_secondBackground);
        if (firstB.position.x - 10 <= -960)
            firstB.position.x = 960;
        else
            firstB.position.x -= 10;
        if (secondB.position.x - 10 <= -960)
            secondB.position.x = 960;
        else
            secondB.position.x -= 10;
    } else
        _oldDt += dt;
}
