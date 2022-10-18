#include "core/Coordinator.hpp"
#include "core/WindowManager.hpp"
#include "core/NetworkManager.hpp"
#include "components/Movement.hpp"
#include "components/Player.hpp"
#include "components/RigidBody.hpp"
#include "components/Sprite.hpp"
#include "components/Transform.hpp"
#include "components/NetworkEntity.hpp"
#include "systems/BackgroundSystem.hpp"
#include "systems/PhysicsSystem.hpp"
#include "systems/PlayerSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/MovementSystem.hpp"
#include "systems/ObjectsSystem.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <loguru.hpp>
#include <ctime>
#include <chrono>
#include <thread>

Coordinator gCoordinator;
std::shared_ptr<ObjectsSystem> gObjectsSystem;
NetworkManager *gNetworkManager;

void NetworkLoop(NetworkManager *networkManager)
{
    gNetworkManager = networkManager;
    networkManager->run();
}

void GameLoop(const char *host, uint16_t port)
{
    NetworkManager networkManager(host, port);
    std::thread threadNetworkLoop(NetworkLoop, &networkManager);
    const std::shared_ptr<WindowManager> windowManager = std::make_shared<WindowManager>();

    windowManager->Init("R-Type", 1920, 700);

    gCoordinator.RegisterComponent<Movement>();
    gCoordinator.RegisterComponent<NetworkEntity>();
    gCoordinator.RegisterComponent<Player>();
    gCoordinator.RegisterComponent<RigidBody>();
    gCoordinator.RegisterComponent<Sprite>();
    gCoordinator.RegisterComponent<Transform>();

    auto renderSystem = gCoordinator.RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Sprite>());
        signature.set(gCoordinator.GetComponentType<Transform>());
        gCoordinator.SetSystemSignature<RenderSystem>(signature);
    }
    renderSystem->Init();

    auto movementSystem = gCoordinator.RegisterSystem<MovementSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Movement>());
        signature.set(gCoordinator.GetComponentType<Transform>());
        gCoordinator.SetSystemSignature<MovementSystem>(signature);
    }
    movementSystem->Init();

    auto physicsSystem = gCoordinator.RegisterSystem<PhysicsSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Movement>());
        signature.set(gCoordinator.GetComponentType<Transform>());
        signature.set(gCoordinator.GetComponentType<RigidBody>());
        gCoordinator.SetSystemSignature<PhysicsSystem>(signature);
    }
    physicsSystem->Init(1920, 700);

    gObjectsSystem = gCoordinator.RegisterSystem<ObjectsSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<NetworkEntity>());
        gCoordinator.SetSystemSignature<ObjectsSystem>(signature);
    }

    auto playerSystem = gCoordinator.RegisterSystem<PlayerSystem>();
    playerSystem->Init();
    gObjectsSystem->Init(playerSystem->GetEntityId());

    auto backgroundSystem = gCoordinator.RegisterSystem<BackgroundSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Transform>());
        gCoordinator.SetSystemSignature<BackgroundSystem>(signature);
    }
    backgroundSystem->Init();

    bool                                  running = true;
    const double                          fps     = 60;
    std::chrono::system_clock::time_point start;
    while (running) {
        start   = std::chrono::system_clock::now();
        running = windowManager->ManageEvent(networkManager);

        // Process packets.
        networkManager.processPackets();

        // Update game.
        playerSystem->Update(windowManager->GetInputs());
        backgroundSystem->Update();
        movementSystem->Update();
        physicsSystem->Update();
        renderSystem->Update(windowManager);
        windowManager->Update();

        const std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - start;
        const std::chrono::duration<double, std::milli> delta_ms((1000 / fps) - work_time.count());
        auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
    }

    networkManager.close();
    threadNetworkLoop.join();

    // We call gCoordinator.Clear() before exiting the program, so as to make sure that destructors for the game objects
    // run before the SFML is destroyed at the global level
    gCoordinator.Clear();
}

int main(int argc, char *argv[])
{
    loguru::init(argc, argv);
    loguru::add_file("r-type_client.log", loguru::Append, loguru::Verbosity_MAX);
    LOG_F(INFO, "Starting client");

    // Check arguments.
    if (argc < 3) {
        std::cout << "Try: " << argv[0] << " [host] [port]." << std::endl;
        return (0);
    }

    // Start game.
    GameLoop(argv[1], std::atoi(argv[2]));

    return (0);
}
