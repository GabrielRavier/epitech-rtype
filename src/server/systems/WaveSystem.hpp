/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WaveSystem
*/

#pragma once

#include <random>
#include "../../client/core/System.hpp"
#include "../../client/core/Coordinator.hpp"
#include "../components/Transform.hpp"
#include "../components/Movement.hpp"
#include "../components/Enemy.hpp"
#include "../components/Weapon.hpp"

class WaveSystem : public System
{
public:
    void Init();
    void Update();

private:
    void                                  CreateWave();
    void                                  CreateBlop(int i);
    std::mt19937                          _mt;
    std::uniform_real_distribution<float> _dist;
};