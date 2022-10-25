/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** SceneManager
*/

#pragma once

#include "../core/Coordinator.hpp"
#include "../core/WindowManager.hpp"
#include "../core/NetworkManager.hpp"
#include "../components/Level.hpp"
#include "../components/Movement.hpp"
#include "../components/Player.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Sprite.hpp"
#include "../components/Transform.hpp"
#include "../components/NetworkEntity.hpp"
#include "../systems/BackgroundSystem.hpp"
#include "../systems/LevelsSystem.hpp"
#include "../systems/MainMenuSystem.hpp"
#include "../systems/MovementSystem.hpp"
#include "../systems/ObjectsSystem.hpp"
#include "../systems/PhysicsSystem.hpp"
#include "../systems/PlayerSystem.hpp"
#include "../systems/RenderSystem.hpp"

class SceneManager
{
public:
    void        Init(char *ip, int port);
    void        LoadScene();
    static void LoadComponents();
    void        LoadSystems();
    void        InitSystems(int windowWidth, int windowHeight);
    void        InitScene();
    void        Loop();
    void        Clear();
    SCENE       MainMenuScene();
    SCENE       SinglePlayerScene();
    SCENE       MultipPlayerScene();
    SCENE       LevelsMenuScene();

private:
    char                          *_ip;
    int                            _port;
    bool                           _running       = true;
    std::shared_ptr<WindowManager> _windowManager = std::make_shared<WindowManager>();
    SCENE                          _currentScene  = SCENE::MAINMENU;
    SCENE                          _oldScene      = SCENE::MAINMENU;
    SCENE(SceneManager::*_method_function[4])
    () = {&SceneManager::MainMenuScene, &SceneManager::MultipPlayerScene, &SceneManager::MultipPlayerScene, &SceneManager::LevelsMenuScene};
    NetworkManager                    _networkManager;
    std::thread                       _threadNetworkLoop;
    std::shared_ptr<BackgroundSystem> _backgroundSystem;
    std::shared_ptr<LevelsSystem>     _levelsSystem;
    std::shared_ptr<MainMenuSystem>   _mainMenuSystem;
    std::shared_ptr<MovementSystem>   _movementSystem;
    std::shared_ptr<PhysicsSystem>    _physicsSystem;
    std::shared_ptr<PlayerSystem>     _playerSystem;
    std::shared_ptr<RenderSystem>     _renderSystem;
};
