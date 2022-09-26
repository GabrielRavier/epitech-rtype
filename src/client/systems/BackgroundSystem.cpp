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
            .rectSize = sf::Vector2i(960, 540),
            .rectPos = sf::Vector2i(0, 0),
            .layer = 0
        }
    );
    gCoordinator.AddComponent(
        _firstBackground,
        Transform{
            .position = sf::Vector2f(0, 0),
            .scale = sf::Vector2f(1, 1),
            .rotation = 0
        }
    );
    gCoordinator.AddComponent(
        _firstBackground,
        Movement{
            .movement = sf::Vector2f(-1, 0),
            .speed = 10
        }
    );

    _secondBackground = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(
        _secondBackground,
        Sprite{
            .texture = secTexture,
            .sprite = secSprite,
            .rectSize = sf::Vector2i(960, 540),
            .rectPos = sf::Vector2i(0, 0),
            .layer = 0
        }
    );
    gCoordinator.AddComponent(
        _secondBackground,
        Transform{
            .position = sf::Vector2f(960, 0),
            .scale = sf::Vector2f(1, 1),
            .rotation = 0
        }
    );
    gCoordinator.AddComponent(
        _secondBackground,
        Movement{
            .movement = sf::Vector2f(-1, 0),
            .speed = 10
        }
    );
}

void BackgroundSystem::Update(float dt)
{
    /*if (dt * 1000 + _oldDt > 2) {
        _oldDt = _oldDt + dt * 1000 - 2;
        auto& firstB = gCoordinator.GetComponent<Transform>(_firstBackground);
        auto& secondB = gCoordinator.GetComponent<Transform>(_secondBackground);
        if (firstB.position.x - 10 <= -960)
            firstB.position.x = 960;
        if (secondB.position.x - 10 <= -960)
            secondB.position.x = 960;
    } else
        _oldDt += dt * 1000;*/
}