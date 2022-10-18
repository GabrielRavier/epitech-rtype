/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WaveSystem
*/

#include "WaveSystem.hpp"
#include "../core/NetworkServerManager.hpp"

extern Coordinator gCoordinator;
extern NetworkServerManager *gServerManager;

void WaveSystem::Init()
{
    std::random_device rd;
    _mt   = std::mt19937(rd());
    _dist = std::uniform_real_distribution<float>(0.0, 400.0);
}

void WaveSystem::Update()
{
    if (mEntities.empty())
        CreateWave();

    for (auto const &entity : mEntities) {
        if (int(_dist(_mt)) / 2 == 1) {
            auto &weapon    = gCoordinator.GetComponent<Weapon>(entity);
            weapon.haveShot = true;
        }
    }
}

void WaveSystem::CreateWave()
{
    for (int i = 0; i < 3; i++)
        CreateBlop(i);
}

void WaveSystem::CreateBlop(int i)
{
    Entity entity   = gCoordinator.CreateEntity();
    int16_t posX    = static_cast<int16_t>(1920 + i * 100);
    int16_t posY    = static_cast<int16_t>(_dist(_mt) + 100);

    gCoordinator.AddComponent<Enemy>(entity, Enemy { Enemy::EnemyType::BLOP, 10 });
    gCoordinator.AddComponent<Weapon>(entity, Weapon {4, 0, Weapon::Type::MISSILETHROWER, Weapon::Team::ENEMY, false });
    gCoordinator.AddComponent<Movement>(entity, Movement { -1, 0, 4 });
    gCoordinator.AddComponent<Transform>(entity, Transform { EntityType::MOB, posX, posY });

    // Broadcast mob create.
    gServerManager->broadcast(new PacketServerEntityCreate(EntityType::MOB, EntityTeam::ENEMY, entity, posX, posY, 1));
}
