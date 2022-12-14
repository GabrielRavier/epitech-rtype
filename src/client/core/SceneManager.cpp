/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** SceneManager
*/

#include "SceneManager.hpp"
#include <thread>

extern Coordinator                    gCoordinator;
extern std::shared_ptr<ObjectsSystem> gObjectsSystem;
extern NetworkManager                *gNetworkManager;

SceneManager::~SceneManager()
{
    if (_currentScene == SCENE::MULTIPLAYER) {
        _networkManager.close();
        if (_threadNetworkLoop.joinable())
            _threadNetworkLoop.join();
    }
    gCoordinator.Clear();
}

void SceneManager::Clear()
{
    if (_currentScene == SCENE::MULTIPLAYER) {
        _networkManager.close();
        if (_threadNetworkLoop.joinable())
            _threadNetworkLoop.join();
    }
    gCoordinator.Clear();
}

void SceneManager::Init(char *ip, int port)
{
    _ip   = ip;
    _port = port;
}

void SceneManager::LoadComponents()
{
    gCoordinator.RegisterComponent<Enemy>();
    gCoordinator.RegisterComponent<Level>();
    gCoordinator.RegisterComponent<NetworkEntity>();
    gCoordinator.RegisterComponent<Movement>();
    gCoordinator.RegisterComponent<Player>();
    gCoordinator.RegisterComponent<Projectile>();
    gCoordinator.RegisterComponent<RigidBody>();
    gCoordinator.RegisterComponent<Sprite>();
    gCoordinator.RegisterComponent<Text>();
    gCoordinator.RegisterComponent<Transform>();
    gCoordinator.RegisterComponent<Weapon>();
}

void SceneManager::LoadSystems()
{
    _renderSystem = gCoordinator.RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Sprite>());
        signature.set(gCoordinator.GetComponentType<Transform>());
        gCoordinator.SetSystemSignature<RenderSystem>(signature);
    }

    if (_currentScene == SCENE::MULTIPLAYER) {
        gObjectsSystem = gCoordinator.RegisterSystem<ObjectsSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<NetworkEntity>());
            gCoordinator.SetSystemSignature<ObjectsSystem>(signature);
        }

        _movementSystem = gCoordinator.RegisterSystem<MovementSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Movement>());
            signature.set(gCoordinator.GetComponentType<Transform>());
            gCoordinator.SetSystemSignature<MovementSystem>(signature);
        }

        _physicsSystem = gCoordinator.RegisterSystem<PhysicsSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Movement>());
            signature.set(gCoordinator.GetComponentType<Transform>());
            signature.set(gCoordinator.GetComponentType<RigidBody>());
            gCoordinator.SetSystemSignature<PhysicsSystem>(signature);
        }

        _playerSystem = gCoordinator.RegisterSystem<PlayerSystem>();

        _backgroundSystem = gCoordinator.RegisterSystem<BackgroundSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Transform>());
            gCoordinator.SetSystemSignature<BackgroundSystem>(signature);
        }
    } else if (_currentScene == SCENE::SOLO) {
        _movementSystem = gCoordinator.RegisterSystem<MovementSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Movement>());
            signature.set(gCoordinator.GetComponentType<Transform>());
            gCoordinator.SetSystemSignature<MovementSystem>(signature);
        }

        _physicsSystem = gCoordinator.RegisterSystem<PhysicsSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Transform>());
            signature.set(gCoordinator.GetComponentType<RigidBody>());
            gCoordinator.SetSystemSignature<PhysicsSystem>(signature);
        }

        _playerSystem = gCoordinator.RegisterSystem<PlayerSystem>();

        _backgroundSystem = gCoordinator.RegisterSystem<BackgroundSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Transform>());
            gCoordinator.SetSystemSignature<BackgroundSystem>(signature);
        }

        _waveSystem = gCoordinator.RegisterSystem<WaveSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Enemy>());
            gCoordinator.SetSystemSignature<WaveSystem>(signature);
        }

        _weaponSystem = gCoordinator.RegisterSystem<WeaponSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Weapon>());
            gCoordinator.SetSystemSignature<WeaponSystem>(signature);
        }
    } else if (_currentScene == SCENE::MAINMENU)
        _mainMenuSystem = gCoordinator.RegisterSystem<MainMenuSystem>();
    else if (_currentScene == SCENE::LEVELSMENU) {
        _levelsSystem = gCoordinator.RegisterSystem<LevelsSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Level>());
            gCoordinator.SetSystemSignature<LevelsSystem>(signature);
        }
        _textSystem = gCoordinator.RegisterSystem<TextSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Text>());
            signature.set(gCoordinator.GetComponentType<Transform>());
            gCoordinator.SetSystemSignature<TextSystem>(signature);
        }
    }
}

void SceneManager::InitSystems(int windowWidth, int windowHeight)
{
    _renderSystem->Init();
    if (_currentScene == SCENE::MULTIPLAYER) {
        _movementSystem->Init();
        _physicsSystem->Init(windowWidth, windowHeight);
        _playerSystem->Init(_currentScene);
        gObjectsSystem->Init(_playerSystem->GetEntityId());
        _backgroundSystem->Init();
    } else if (_currentScene == SCENE::SOLO) {
        _movementSystem->Init();
        _physicsSystem->Init(windowWidth, windowHeight);
        _weaponSystem->Init();
        _waveSystem->Init(_levelPath);
        _playerSystem->Init(_currentScene);
        _backgroundSystem->Init();
    } else if (_currentScene == SCENE::MAINMENU) {
        _mainMenuSystem->Init();
    } else if (_currentScene == SCENE::LEVELSMENU) {
        _levelsSystem->Init();
    }
}

void NetworkLoop(NetworkManager *networkManager)
{
    networkManager->run();
}

void SceneManager::LoadScene()
{
    if (_oldScene == SCENE::MULTIPLAYER) {
        _networkManager.close();
        _threadNetworkLoop.join();
    }
    _oldScene = _currentScene;
    gCoordinator.Clear();
    if (_currentScene == SCENE::MULTIPLAYER) {
        _networkManager.Init(_ip, _port);
        _threadNetworkLoop = std::thread(NetworkLoop, &_networkManager);
        gNetworkManager    = &_networkManager;
    }
    LoadComponents();
    LoadSystems();
    InitSystems(1920, 700);
}

void SceneManager::Loop()
{
    _windowManager->Init("R-Type", 1920, 700);

    const double                          fps = 60;
    std::chrono::system_clock::time_point start;
    SCENE                                 newScene;
    LoadScene();

    while (_running) {
        start                  = std::chrono::system_clock::now();
        auto methodFunctionPtr = _method_function[_currentScene];
        newScene               = std::invoke(methodFunctionPtr, this);
        if (newScene != _currentScene) {
            _currentScene = newScene;
            LoadScene();
        }
        _windowManager->Update();
        const std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - start;
        const std::chrono::duration<double, std::milli> delta_ms((1000 / fps) - work_time.count());
        auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
    }
}

SCENE SceneManager::MainMenuScene()
{
    _running = _windowManager->ManageEvent();
    _renderSystem->Update(_windowManager, false);
    auto scene = _mainMenuSystem->Update(_windowManager->GetMousePosition(), _windowManager->MouseClicked());
    return (scene);
}

SCENE SceneManager::MultipPlayerScene()
{
    _running = _windowManager->ManageNetworkEvent(_networkManager);
    _networkManager.processPackets();

    _playerSystem->Update(_windowManager->GetInputs());
    _backgroundSystem->Update();
    _movementSystem->Update();
    _physicsSystem->Update();
    _renderSystem->Update(_windowManager, true);
    return (SCENE::MULTIPLAYER);
}

SCENE SceneManager::SinglePlayerScene()
{
    SCENE scene = SCENE::SOLO;
    _running    = _windowManager->ManageEvent();

    _playerSystem->Update(_windowManager->GetInputs());
    _weaponSystem->Update();
    _backgroundSystem->Update();
    _movementSystem->Update();
    scene = _waveSystem->Update();
    _physicsSystem->ClientUpdate();
    _renderSystem->Update(_windowManager, false);
    scene = _playerSystem->checkPlayersLife(scene);
    return (scene);
}

SCENE SceneManager::LevelsMenuScene()
{
    _running = _windowManager->ManageEvent();
    _renderSystem->Update(_windowManager, false);
    _textSystem->Update(_windowManager);
    auto scene = _levelsSystem->Update(_windowManager->GetMousePosition(), _windowManager->MouseClicked(), &_levelPath);
    return (scene);
}
