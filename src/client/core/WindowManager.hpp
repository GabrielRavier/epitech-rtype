/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WindowManager
*/

#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <bitset>
#include <memory>
#include <vector>
#include <unordered_map>
#include "NetworkManager.hpp"

class WindowManager
{
public:
    void Init(const sf::String &title, int width, int height);
    void Clear();
    void Update();
    void RenderSprite(const std::shared_ptr<sf::Sprite> &sprite, sf::Vector2f position);
    bool ManageEvent(NetworkManager &manager);
    std::bitset<8> GetInputs() const;

private:
    std::unique_ptr<sf::RenderWindow>   _window;
    std::bitset<8>                      _inputs;
};
