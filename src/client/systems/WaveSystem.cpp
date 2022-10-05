/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WaveSystem
*/

#include "WaveSystem.hpp"
#include <iostream>

extern Coordinator gCoordinator;

void WaveSystem::Init()
{
    std::random_device rd;
    _mt   = std::mt19937(rd());
    _dist = std::uniform_real_distribution<float>(0.0, 400.0);
}

void WaveSystem::Update()
{
    if (mEntities.empty())
        CreateWave();
    for (auto const &entity : mEntities) {
        if (int(_dist(_mt)) / 2 == 1) {
            auto &weapon    = gCoordinator.GetComponent<Weapon>(entity);
            weapon.haveShot = true;
        }
    }
}

void WaveSystem::CreateWave()
{
    for (int i = 0; i < 3; i += 1)
        CreateBlop(i);
}

void WaveSystem::CreateBlop(int i)
{
    const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
    const sf::Vector2f                 scale   = sf::Vector2f(3, 3);
    texture->loadFromFile("./assets/blop.gif");
    sprite->setTexture(*texture, false);
    sprite->setScale(scale);
    sprite->setTextureRect(sf::IntRect(5, 34, 24, 34));
    auto blop = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(blop, Weapon{4, 0, Weapon::Type::MISSILETHROWER, Weapon::Team::ENEMY, false});
    gCoordinator.AddComponent(blop, Movement{sf::Vector2f(-1.f, 0.f), 3});
    gCoordinator.AddComponent(blop, Enemy{Enemy::EnemyType::BLOP, 10});
    gCoordinator.AddComponent(blop, Sprite{texture, sprite, sf::Vector2i(24, 34), sf::Vector2i(5, 34), 1});
    gCoordinator.AddComponent(blop,
                              Transform{sf::Vector2f(float(1920 + i * 100), float(int(_dist(_mt)) + 100)), scale, 0});
}
