/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WaveSystem
*/

#include "WaveSystem.hpp"
#include "../core/NetworkServerManager.hpp"

extern Coordinator           gCoordinator;
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
        if (int(_dist(_mt)) % 100 == 2) {
            auto &weapon    = gCoordinator.GetComponent<Weapon>(entity);
            weapon.haveShot = true;
        }
    }
}

void WaveSystem::CreateWave()
{
    const int waveId = int(_dist(_mt)) % 3;

    if (waveId == 0) {
        for (int i = 0; i < 5; i++)
            CreateBlop(i);
    } else if (waveId == 1) {
        for (int i = 0; i < 5; i++)
            CreateCrop(i);
    } else
        CreateBoss(0);
}

void WaveSystem::CreateBlop(int i)
{
    const Entity entity = gCoordinator.CreateEntity();
    auto         posX   = static_cast<int16_t>(1920 + i * 100);
    auto         posY   = static_cast<int16_t>(_dist(_mt) + 100);

    gCoordinator.AddComponent<Enemy>(entity, Enemy{Enemy::EnemyType::BLOP, 5});
    gCoordinator.AddComponent<Weapon>(entity, Weapon{25, 0, Weapon::Type::MISSILETHROWER, Weapon::Team::ENEMY, false});
    gCoordinator.AddComponent<Movement>(entity, Movement{-1, 0, 4});
    gCoordinator.AddComponent<Transform>(entity, Transform{EntityType::MOB, MobType::BLOP, posX, posY});
    gCoordinator.AddComponent<RigidBody>(entity, RigidBody{23 * 3, 22 * 3});

    // Broadcast mob create.
    gServerManager->broadcast(
        new PacketServerEntityCreate(EntityType::MOB, EntityTeam::ENEMY, MobType::BLOP, entity, posX, posY, 1));
}

void WaveSystem::CreateCrop(int i)
{
    const Entity entity = gCoordinator.CreateEntity();
    auto         posX   = static_cast<int16_t>(1920 + i * 100);
    auto         posY   = static_cast<int16_t>(_dist(_mt) + 100);
    gCoordinator.AddComponent<Enemy>(entity, Enemy{Enemy::EnemyType::CROP, 10});
    gCoordinator.AddComponent<Weapon>(entity, Weapon{25, 0, Weapon::Type::MISSILETHROWER, Weapon::Team::ENEMY, false});
    gCoordinator.AddComponent<Movement>(entity, Movement{-1, 0, 4});
    gCoordinator.AddComponent<Transform>(entity, Transform{EntityType::MOB, MobType::CROP, posX, posY});
    gCoordinator.AddComponent<RigidBody>(entity, RigidBody{33 * 3, 32 * 3});

    gServerManager->broadcast(
        new PacketServerEntityCreate(EntityType::MOB, EntityTeam::ENEMY, MobType::CROP, entity, posX, posY, 1));
}

void WaveSystem::CreateBoss(int i)
{
    (void)i; // Unused for now

    const Entity entity = gCoordinator.CreateEntity();
    auto         posX   = static_cast<int16_t>(1200);
    auto         posY   = static_cast<int16_t>(0);
    gCoordinator.AddComponent<Enemy>(entity, Enemy{Enemy::EnemyType::BOSS, 50});
    gCoordinator.AddComponent<Weapon>(entity, Weapon{80, 0, Weapon::Type::MISSILETHROWER, Weapon::Team::ENEMY, false});
    gCoordinator.AddComponent<Transform>(entity, Transform{EntityType::MOB, MobType::BOSS, posX, posY});
    gCoordinator.AddComponent<RigidBody>(entity, RigidBody{180 * 3, 210 * 3});

    gServerManager->broadcast(
        new PacketServerEntityCreate(EntityType::MOB, EntityTeam::ENEMY, MobType::BOSS, entity, posX, posY, 1));
}
