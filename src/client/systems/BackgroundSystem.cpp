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
    const std::shared_ptr<sf::Texture> firTexture = std::make_shared<sf::Texture>();
    firTexture->loadFromFile("../assets/background.png");
    const std::shared_ptr<sf::Sprite> firSprite = std::make_shared<sf::Sprite>();
    firSprite->setTexture(*firTexture, false);
    const std::shared_ptr<sf::Texture> secTexture = std::make_shared<sf::Texture>();
    secTexture->loadFromFile("../assets/background.png");
    const std::shared_ptr<sf::Sprite> secSprite = std::make_shared<sf::Sprite>();
    secSprite->setTexture(*secTexture, false);
    firSprite->setScale(sf::Vector2f(2, 2));
    secSprite->setScale(sf::Vector2f(2, 2));

    _firstBackground = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(_firstBackground, Sprite{
                                                    firTexture,             /* texture */
                                                    firSprite,              /* sprite */
                                                    sf::Vector2i(960, 540), /* rectSize */
                                                    sf::Vector2i(0, 0),     /* rectPos */
                                                    0,                      /* layer */
                                                });
    gCoordinator.AddComponent(_firstBackground, Transform{
                                                    sf::Vector2f(0, 0), /* position */
                                                    sf::Vector2f(1, 1), /* scale */
                                                    0,                  /* rotation */
                                                });
    gCoordinator.AddComponent(_firstBackground, Movement{
                                                    sf::Vector2f(-1, 0), /* movement */
                                                    5,                   /* speed */
                                                });

    _secondBackground = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(_secondBackground, Sprite{
                                                     secTexture,             /* texture */
                                                     secSprite,              /* sprite */
                                                     sf::Vector2i(960, 540), /* rectSize */
                                                     sf::Vector2i(0, 0),     /* rectPos */
                                                     0,                      /* layer */
                                                 });
    gCoordinator.AddComponent(_secondBackground, Transform{
                                                     sf::Vector2f(1920, 0), /* position */
                                                     sf::Vector2f(1, 1),    /* scale */
                                                     0,                     /* rotation */
                                                 });
    gCoordinator.AddComponent(_secondBackground, Movement{
                                                     sf::Vector2f(-1, 0), /* movement */
                                                     5,                   /* speed */
                                                 });
}

void BackgroundSystem::Update() const
{
    auto &firstB  = gCoordinator.GetComponent<Transform>(_firstBackground);
    auto &secondB = gCoordinator.GetComponent<Transform>(_secondBackground);
    if (firstB.position.x <= -1920)
        firstB.position.x = 1920;
    if (secondB.position.x <= -1920)
        secondB.position.x = 1920;
}
