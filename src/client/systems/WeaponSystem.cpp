/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WeaponSystem
*/

#include "WeaponSystem.hpp"
#include <iostream>

extern Coordinator gCoordinator;

void WeaponSystem::Init() {}

void WeaponSystem::Update() const
{
    for (auto const &entity : mEntities) {
        auto       &weapon    = gCoordinator.GetComponent<Weapon>(entity);
        auto const &transform = gCoordinator.GetComponent<Transform>(entity);
        if (weapon.haveShot && weapon.shootTimer == 0) {
            weapon.shootTimer = maxRateOfFire - weapon.rateOfFire;
            createMissileProjectile(weapon, transform);
        } else if (weapon.shootTimer > 0)
            weapon.shootTimer -= 1;
        weapon.haveShot = false;
    }
}

void WeaponSystem::createMissileProjectile(Weapon weapon, Transform transform)
{
    const std::shared_ptr<sf::Texture> texture  = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  sprite   = std::make_shared<sf::Sprite>();
    sf::Vector2f                       position = transform.position;
    position.x += 20;
    texture->loadFromFile("./assets/projectiles.gif");
    sprite->setTexture(*texture, false);
    sprite->setScale(sf::Vector2f(3, 3));
    sprite->setTextureRect(sf::IntRect(248, 100, 16, 16));
    auto tprojectile = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(tprojectile, Projectile{10, weapon.team});
    gCoordinator.AddComponent(tprojectile, Movement{sf::Vector2f(1, 0), 40});
    gCoordinator.AddComponent(tprojectile, Sprite{texture, sprite, sf::Vector2i(50, 50), sf::Vector2i(250, 100), 1});
    gCoordinator.AddComponent(tprojectile, Transform{transform.position, sf::Vector2f(3, 3), 0});
}
