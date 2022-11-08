/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WaveSystem
*/

#include "WaveSystem.hpp"

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
        if (int(_dist(_mt)) % 100 == 2) {
            auto &weapon    = gCoordinator.GetComponent<Weapon>(entity);
            weapon.haveShot = true;
        }
    }
}

void WaveSystem::CreateWave()
{
    for (int i = 0; i < 3; i++)
        CreateBlop(i);
}

void WaveSystem::CreateBlop(int i)
{
    const sf::Vector2f                 position(1920 + i * 100, _dist(_mt) + 100);
    const Entity                       entity  = gCoordinator.CreateEntity();
    const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
    const sf::Vector2f                 scale   = sf::Vector2f(3, 3);

    texture->loadFromFile("./assets/blop.gif");
    sprite->setTexture(*texture, false);
    sprite->setScale(scale);
    sprite->setTextureRect(sf::IntRect(6, 40, 23, 22));

    gCoordinator.AddComponent<Sprite>(entity, Sprite{texture, sprite, sf::Vector2i(23, 22), sf::Vector2i(6, 40), 1});
    gCoordinator.AddComponent<Enemy>(entity, Enemy{Enemy::EnemyType::BLOP, 10});
    gCoordinator.AddComponent<Weapon>(entity, Weapon{1, 0, Weapon::Type::MISSILETHROWER, Weapon::Team::ENEMY, false});
    gCoordinator.AddComponent<Movement>(entity, Movement{sf::Vector2f(-1, 0), 2});
    gCoordinator.AddComponent<Transform>(entity, Transform{position, sf::Vector2f(3, 3), 0});
    gCoordinator.AddComponent<RigidBody>(entity,
                                         RigidBody{sf::Vector2f(23 * scale.x, 22 * scale.y), RigidBody::Type::ENEMY});
}

/*void WaveSystem::CreateCrop(int i)
{
    const sf::Vector2f                 position(1920 + i * 100, _dist(_mt) + 100);
    const Entity                       entity  = gCoordinator.CreateEntity();
    const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
    const sf::Vector2f                 scale   = sf::Vector2f(3, 3);

    texture->loadFromFile("./assets/crop.gif");
    sprite->setTexture(*texture, false);
    sprite->setScale(scale);
    sprite->setTextureRect(sf::IntRect(1, 29, 29, 29));

    gCoordinator.AddComponent<Sprite>(entity, Sprite{texture, sprite, sf::Vector2i(29, 29), sf::Vector2i(1, 29), 1});
    gCoordinator.AddComponent<Enemy>(entity, Enemy{Enemy::EnemyType::CROP, 10});
    gCoordinator.AddComponent<Weapon>(entity, Weapon{4, 0, Weapon::Type::MISSILETHROWER, Weapon::Team::ENEMY, false});
    gCoordinator.AddComponent<Movement>(entity, Movement{sf::Vector2f(-1, 0), 2});
    gCoordinator.AddComponent<Transform>(entity, Transform{position, sf::Vector2f(3, 3), 0});
    gCoordinator.AddComponent<RigidBody>(entity, RigidBody{sf::Vector2f(29, 29)});
}*/
