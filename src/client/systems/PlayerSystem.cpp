/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** PlayerSystem
*/

#include <iostream>
#include "PlayerSystem.hpp"
#include "../../server/packets/packet_client_input.hpp"

extern Coordinator gCoordinator;

void PlayerSystem::Init()
{
    const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
    texture->loadFromFile("./assets/players.gif");
    sprite->setTexture(*texture, false);
    sprite->setScale(sf::Vector2f(3, 3));
    sprite->setTextureRect(sf::IntRect(0, 0, 33, 17));

    _player = gCoordinator.CreateEntity();
    std::vector<Entity> weapons;
    auto                weapon = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(weapon, Weapon{30, 0, Weapon::Type::MISSILETHROWER, Weapon::Team::PLAYERS, false});
    gCoordinator.AddComponent(weapon, Transform{sf::Vector2f(50, 50), sf::Vector2f(1, 1), 0});
    weapons.push_back(weapon);
    gCoordinator.AddComponent(_player, Sprite{texture, sprite, sf::Vector2i(33, 17), sf::Vector2i(0, 0), 1});
    gCoordinator.AddComponent(_player, Transform{sf::Vector2f(50, 50), sf::Vector2f(3, 3), 0});
    gCoordinator.AddComponent(_player, Player{"Player One", 10, 10, weapons});
    gCoordinator.AddComponent(_player, Movement{sf::Vector2f(0, 0), 5});
    gCoordinator.AddComponent(_player, RigidBody{sf::Vector2f(33, 17)});
}

void PlayerSystem::Update(std::bitset<8> inputs) const
{
    auto &movement = gCoordinator.GetComponent<Movement>(_player);
    float x        = 0;
    float y        = 0;

    if (inputs.test(InputType::RIGHT))
        x += 1;
    if (inputs.test(InputType::LEFT))
        x -= 1;
    if (inputs.test(InputType::DOWN))
        y += 1;
    if (inputs.test(InputType::UP))
        y -= 1;
    if (inputs.test(InputType::SHOOT))
        this->Shoot();
    movement.movement.x = x;
    movement.movement.y = y;
    this->ChangeWeaponsPosition();
}

void PlayerSystem::Shoot() const
{
    auto const &player = gCoordinator.GetComponent<Player>(_player);

    for (auto const &weapon : player.weapons) {
        auto &weaponComponent    = gCoordinator.GetComponent<Weapon>(weapon);
        weaponComponent.haveShot = true;
    }
}

void PlayerSystem::ChangeWeaponsPosition() const
{
    auto const &player     = gCoordinator.GetComponent<Player>(_player);
    auto const &ptransform = gCoordinator.GetComponent<Transform>(_player);

    for (auto const &weapon : player.weapons) {
        auto &wtransform    = gCoordinator.GetComponent<Transform>(weapon);
        wtransform.position = ptransform.position;
    }
}
