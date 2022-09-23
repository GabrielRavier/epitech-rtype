/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WindowSystem
*/

#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "../core/System.hpp"
#include "../core/Coordinator.hpp"

class WindowSystem : public System
{
public:
    void Init();
    void Update();
    void Shutdown();
private:
    Entity window;
};