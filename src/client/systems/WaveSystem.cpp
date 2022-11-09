/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** WaveSystem
*/

#include "WaveSystem.hpp"
#include <iostream>

extern Coordinator gCoordinator;

void WaveSystem::Init(std::string levelPath)
{
    std::random_device rd;
    _mt        = std::mt19937(rd());
    _dist      = std::uniform_real_distribution<float>(0.0, 400.0);
    _levelPath = levelPath;
    _waveIndex = 0;
    _finish    = false;
}

SCENE WaveSystem::Update()
{
    if (_finish) {
        MarkLevelDone();
        return (SCENE::MAINMENU);
    }
    if (mEntities.empty() && !_finish)
        ReadWave();

    for (auto const &entity : mEntities) {
        if (int(_dist(_mt)) % 100 == 2) {
            auto &weapon    = gCoordinator.GetComponent<Weapon>(entity);
            weapon.haveShot = true;
        }
    }
    return (SCENE::SOLO);
}

void WaveSystem::ReadWave()
{
    std::ifstream file;
    std::string   line;
    std::string   waveString;
    bool          lineToAnalyse = false;

    _waveIndex += 1;
    waveString = "W" + std::to_string(_waveIndex);
    file.open(_levelPath);
    while (file) {
        std::getline(file, line);
        if (lineToAnalyse) {
            CreateWave(line);
            return;
        }
        if (line == waveString)
            lineToAnalyse = true;
    }
    _finish = true;
}

void WaveSystem::MarkLevelDone()
{
    std::ifstream            file;
    std::string              line;
    std::vector<std::string> lines;
    int                      i = 0;

    file.open(_levelPath);
    while (file) {
        std::getline(file, line);
        if (i == 1)
            lines.push_back("1");
        else
            lines.push_back(line);
        i += 1;
    }
    std::ofstream ofile(_levelPath);
    i = 0;
    for (const std::string &l : lines) {
        if (i == lines.size() - 1)
            return;
        ofile << l << std::endl;
        i += 1;
    }
}

void WaveSystem::CreateWave(std::string line)
{
    std::string              delimiter = " ";
    size_t                   pos       = 0;
    std::vector<std::string> mobs{};

    while ((pos = line.find(delimiter)) != std::string::npos) {
        mobs.push_back(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
    }
    mobs.push_back(line.substr(0, pos));

    int i           = 0;
    int methodIndex = 0;

    for (const auto &mob : mobs) {
        methodIndex = std::stoi(mob);
        if (methodIndex >= 1)
            continue;
        (this->*_method_function[methodIndex])(i);
        i += 1;
    }
}

void WaveSystem::CreateBlop(int i)
{
    const sf::Vector2f                 position(1920 + i * 100, _dist(_mt) + 100);
    const Entity                       entity  = gCoordinator.CreateEntity();
    const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
    const sf::Vector2f                 scale   = sf::Vector2f(3, 3);

    texture->loadFromFile("./assets/blop.gif");
    sprite->setTexture(*texture, false);
    sprite->setScale(scale);
    sprite->setTextureRect(sf::IntRect(6, 40, 23, 22));

    gCoordinator.AddComponent<Sprite>(entity, Sprite{texture, sprite, sf::Vector2i(23, 22), sf::Vector2i(6, 40), 1});
    gCoordinator.AddComponent<Enemy>(entity, Enemy{Enemy::EnemyType::BLOP, 10});
    gCoordinator.AddComponent<Weapon>(entity, Weapon{25, 0, Weapon::Type::MISSILETHROWER, Weapon::Team::ENEMY, false});
    gCoordinator.AddComponent<Movement>(entity, Movement{sf::Vector2f(-1, 0), 5});
    gCoordinator.AddComponent<Transform>(entity, Transform{position, sf::Vector2f(3, 3), 0});
    gCoordinator.AddComponent<RigidBody>(entity,
                                         RigidBody{sf::Vector2f(23 * scale.x, 22 * scale.y), RigidBody::Type::ENEMY});
}

void WaveSystem::CreateCrop(int i)
{
    const sf::Vector2f                 position(1920 + i * 100, _dist(_mt) + 100);
    const Entity                       entity  = gCoordinator.CreateEntity();
    const std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  sprite  = std::make_shared<sf::Sprite>();
    const sf::Vector2f                 scale   = sf::Vector2f(3, 3);

    texture->loadFromFile("./assets/crop.gif");
    sprite->setTexture(*texture, false);
    sprite->setScale(scale);
    sprite->setTextureRect(sf::IntRect(1, 29, 29, 29));

    gCoordinator.AddComponent<Sprite>(entity, Sprite{texture, sprite, sf::Vector2i(29, 29), sf::Vector2i(1, 29), 1});
    gCoordinator.AddComponent<Enemy>(entity, Enemy{Enemy::EnemyType::CROP, 10});
    gCoordinator.AddComponent<Weapon>(entity, Weapon{20, 0, Weapon::Type::MISSILETHROWER, Weapon::Team::ENEMY, false});
    gCoordinator.AddComponent<Movement>(entity, Movement{sf::Vector2f(-1, 0), 2});
    gCoordinator.AddComponent<Transform>(entity, Transform{position, sf::Vector2f(3, 3), 0});
    gCoordinator.AddComponent<RigidBody>(entity, RigidBody{sf::Vector2f(29, 29)});
}
