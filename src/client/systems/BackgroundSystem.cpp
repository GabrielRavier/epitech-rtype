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
    gCoordinator.AddComponent(
        _firstBackground,
        Sprite{
            .texture = backgroundTexture,
            .sprite = backgroundSprite,
            .rectSize = Vec2(960, 540),
            .rectPos = Vec2(0, 0),
            .layer = 0
        }
    );
    gCoordinator.AddComponent(
        _firstBackground,
        Transform{
            .position = Vec2(0, 0),
            .scale = Vec2(1, 1),
            .rotation = 0
        }
    );

    _secondBackground = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(
        _secondBackground,
        Sprite{
            .texture = secTexture,
            .sprite = secSprite,
            .rectSize = Vec2(960, 540),
            .rectPos = Vec2(0, 0),
            .layer = 0
        }
    );
    gCoordinator.AddComponent(
        _secondBackground,
        Transform{
            .position = Vec2(960, 0),
            .scale = Vec2(1, 1),
            .rotation = 0
        }
    );
}

void BackgroundSystem::Update(float dt)
{
    if (dt * 1000 + _oldDt > 2) {
        _oldDt = _oldDt + dt * 1000 - 2;
        auto& firstB = gCoordinator.GetComponent<Transform>(_firstBackground);
        auto& secondB = gCoordinator.GetComponent<Transform>(_secondBackground);
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