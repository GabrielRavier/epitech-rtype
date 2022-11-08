/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WeaponSystem
*/

#include "WeaponSystem.hpp"

extern Coordinator gCoordinator;

void WeaponSystem::Init() {}

void WeaponSystem::Update() const
{
    for (auto const &entity : mEntities) {
        auto       &weapon    = gCoordinator.GetComponent<Weapon>(entity);
        auto const &transform = gCoordinator.GetComponent<Transform>(entity);
        auto const &rigidbody = gCoordinator.GetComponent<RigidBody>(entity);

        if (weapon.shootTimer > 0)
            weapon.shootTimer--;

        if (weapon.haveShot) {
            weapon.haveShot = false;

            if (weapon.shootTimer == 0) {
                weapon.shootTimer = 100 - weapon.rateOfFire;
                createMissileProjectile(weapon, transform, rigidbody);
            }
        }
    }
}

void WeaponSystem::createMissileProjectile(Weapon weapon, Transform transform, RigidBody rigidbody)
{
    const Entity                       entity  = gCoordinator.CreateEntity();
    const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
    const sf::Vector2f                 scale   = sf::Vector2f(3.f, 3.f);
    const auto                         rotate  = float(0 + (180 * weapon.team));
    const sf::Vector2f                 pos     = sf::Vector2f(transform.position.x + (rigidbody.hitbox.x / 2),
                                                              transform.position.y + (rigidbody.hitbox.y / 2) + 12);

    texture->loadFromFile("./assets/projectiles.gif");
    sprite->setTexture(*texture, false);
    sprite->setTextureRect(sf::IntRect(248, 105, 16, 8));
    sprite->setScale(scale);
    sprite->setRotation(rotate);

    gCoordinator.AddComponent<Sprite>(entity, Sprite{texture, sprite, sf::Vector2i(16, 8), sf::Vector2i(248, 105), 1});
    gCoordinator.AddComponent<Projectile>(entity, Projectile{1, weapon.team});
    gCoordinator.AddComponent<Transform>(entity, Transform{pos, scale, rotate});
    gCoordinator.AddComponent<Movement>(entity, Movement{sf::Vector2f(1 - (2 * weapon.team), 0), 20});
    gCoordinator.AddComponent<RigidBody>(
        entity, RigidBody{sf::Vector2f(16 * scale.x, 8 * scale.y), RigidBody::Type::PROJECTILE});
}
