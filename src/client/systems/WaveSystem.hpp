/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WaveSystem
*/

#pragma once

#include <random>
#include <string>
#include <fstream>
#include <vector>
#include "../../client/core/System.hpp"
#include "../../client/core/Coordinator.hpp"
#include "../components/Enemy.hpp"
#include "../components/Weapon.hpp"
#include "../components/Transform.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Movement.hpp"
#include "../components/Sprite.hpp"

class WaveSystem : public System
{
public:
    void  Init(std::string levelPath);
    SCENE Update();

private:
    void                                  ReadWave();
    void                                  CreateWave(std::string line);
    void                                  CreateBlop(int i);
    void                                  CreateCrop(int i);
    void                                  MarkLevelDone();
    std::string                           _levelPath;
    int                                   _waveIndex;
    bool                                  _finish;
    std::mt19937                          _mt;
    std::uniform_real_distribution<float> _dist;
    void (WaveSystem::*_method_function[1])(int) = {&WaveSystem::CreateBlop};
};
