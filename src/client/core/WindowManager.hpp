/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WindowManager
*/

#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

class WindowManager
{
public:
    void Init(std::string title, int width, int height);
    void Clear();
    void Update();
    void RenderSprite(std::shared_ptr<sf::Sprite> sprite, sf::Vector2f position);
private:
    std::unique_ptr<sf::RenderWindow> _window;
};