/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** PlayerSystem
*/

#include "PlayerSystem.hpp"

extern Coordinator gCoordinator;

void PlayerSystem::Init(SCENE scene)
{
    const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
    texture->loadFromFile("./assets/players.gif");
    sprite->setTexture(*texture, false);
    sprite->setTextureRect(sf::IntRect(1, 3, 32, 14));
    sprite->setScale(sf::Vector2f(3, 3));

    _player = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(_player, Sprite{texture, sprite, sf::Vector2i(32, 14), sf::Vector2i(1, 3), 1});
    gCoordinator.AddComponent(_player, Transform{sf::Vector2f(50, 50), sf::Vector2f(3, 3), 0});
    gCoordinator.AddComponent(_player, Player{"Player One", 10, 10});
    gCoordinator.AddComponent(_player, Movement{sf::Vector2f(0, 0), 5});
    gCoordinator.AddComponent(_player, RigidBody{sf::Vector2f(32 * 3, 14 * 3), RigidBody::Type::PLAYER});
    if (scene == SCENE::MULTIPLAYER)
        gCoordinator.AddComponent(_player, NetworkEntity{static_cast<Entity>(-1)});
    else if (scene == SCENE::SOLO)
        gCoordinator.AddComponent(_player, Weapon{80, 0, Weapon::Type::MISSILETHROWER, Weapon::Team::PLAYERS, false});
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
    if (inputs.test(InputType::SHOOT)) {
        auto &weapon    = gCoordinator.GetComponent<Weapon>(_player);
        weapon.haveShot = true;
    }
    movement.movement.x = x;
    movement.movement.y = y;
}

SCENE PlayerSystem::checkPlayersLife(SCENE scene) const
{
    const auto &player = gCoordinator.GetComponent<Player>(_player);

    if (player.life <= 0)
        return (SCENE::MAINMENU);
    return (scene);
}

Entity PlayerSystem::GetEntityId() const
{
    return _player;
}
