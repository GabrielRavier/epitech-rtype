/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** BackgroundSystem
*/

#include "BackgroundSystem.hpp"
#include <memory>
#include <iostream>

extern Coordinator gCoordinator;

void BackgroundSystem::Init()
{
    std::shared_ptr<sf::Texture> backgroundTexture = std::make_shared<sf::Texture>();
    backgroundTexture->loadFromFile("./assets/background.png");
    std::shared_ptr<sf::Sprite> backgroundSprite = std::make_shared<sf::Sprite>();
    backgroundSprite->setTexture(*backgroundTexture, false);
    std::shared_ptr<sf::Texture> secTexture = std::make_shared<sf::Texture>();
    secTexture->loadFromFile("./assets/background.png");
    std::shared_ptr<sf::Sprite> secSprite = std::make_shared<sf::Sprite>();
    secSprite->setTexture(*secTexture, false);

    _firstBackground = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(_firstBackground, Sprite{
                                                    backgroundTexture, /* texture */
                                                    backgroundSprite,  /* sprite */
                                                    Vec2(960, 540),    /* rectSize */
                                                    Vec2(0, 0),        /* rectPos */
                                                    0,                 /* layer */
                                                });
    gCoordinator.AddComponent(_firstBackground, Transform{
                                                    Vec2(0, 0), /* position */
                                                    Vec2(1, 1), /* scale */
                                                    0,          /* rotation */
                                                });

    _secondBackground = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(_secondBackground, Sprite{
                                                     secTexture,     /* texture */
                                                     secSprite,      /* sprite */
                                                     Vec2(960, 540), /* rectSize */
                                                     Vec2(0, 0),     /* rectPos */
                                                     0,              /* layer */
                                                 });
    gCoordinator.AddComponent(_secondBackground, Transform{
                                                     Vec2(960, 0), /* position */
                                                     Vec2(1, 1),   /* scale */
                                                     0,            /* rotation */
                                                 });
}

void BackgroundSystem::Update(float dt)
{
    if (dt * 1000 + _oldDt > 2) {
        _oldDt        = _oldDt + dt * 1000 - 2;
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
        _oldDt += dt * 1000;
}
