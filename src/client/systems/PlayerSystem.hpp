/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** PlayerSystem
*/

#pragma once

#include "../core/System.hpp"
#include "../core/Coordinator.hpp"
#include "../components/Player.hpp"
#include "../components/Movement.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Transform.hpp"
#include "../components/Sprite.hpp"
#include "../components/Weapon.hpp"

class PlayerSystem : public System
{
public:
    void Init();
    void Update(std::unordered_map<sf::Keyboard::Key, bool> buttonsPressed) const;
    void Shoot() const;
    void ChangeWeaponsPosition() const;

private:
    Entity _player;
};
