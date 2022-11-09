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
#include "../components/Enemy.hpp"
#include "../components/Weapon.hpp"
#include "../components/Transform.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Movement.hpp"

class WaveSystem : public System
{
public:
    void Init();
    void Update();

private:
    void                                  CreateWave();
    void                                  CreateBlop(int i);
    void                                  CreateCrop(int i);
    void                                  CreateBoss(int i);
    std::mt19937                          _mt;
    std::uniform_real_distribution<float> _dist;
};
