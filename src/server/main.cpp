#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <map>
#include <string>
#include <boost/asio.hpp>
#include "../client/core/Coordinator.hpp"
#include "../client/core/WindowManager.hpp"
#include "../client/core/NetworkManager.hpp"
#include "../client/components/Player.hpp"
#include "core/NetworkServerManager.hpp"
#include "systems/ObjectsSystem.hpp"
#include "systems/PhysicsSystem.hpp"
#include "systems/MovementSystem.hpp"
#include "systems/WaveSystem.hpp"
#include "systems/WeaponSystem.hpp"
#include "components/Enemy.hpp"
#include "components/Network.hpp"
#include "components/Projectile.hpp"
#include "components/RigidBody.hpp"
#include "components/Transform.hpp"
#include "components/Weapon.hpp"

Coordinator                    gCoordinator;
std::shared_ptr<ObjectsSystem> gObjectsSystem;
NetworkServerManager          *gServerManager;

void NetworkLoop(NetworkServerManager *networkManager)
{
    networkManager->run();
}

void GameLoop(uint16_t port)
{
    NetworkServerManager networkManager(port);
    std::thread          threadNetworkLoop(NetworkLoop, &networkManager);

    // Set network manager.
    gServerManager = &networkManager;

    gCoordinator.RegisterComponent<Player>();
    gCoordinator.RegisterComponent<Enemy>();
    gCoordinator.RegisterComponent<Movement>();
    gCoordinator.RegisterComponent<Network>();
    gCoordinator.RegisterComponent<Projectile>();
    gCoordinator.RegisterComponent<RigidBody>();
    gCoordinator.RegisterComponent<Transform>();
    gCoordinator.RegisterComponent<Weapon>();

    gObjectsSystem = gCoordinator.RegisterSystem<ObjectsSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Transform>());
        gCoordinator.SetSystemSignature<ObjectsSystem>(signature);
    }

    auto waveSystem = gCoordinator.RegisterSystem<WaveSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Enemy>());
        signature.set(gCoordinator.GetComponentType<Weapon>());
        gCoordinator.SetSystemSignature<WaveSystem>(signature);
    }
    waveSystem->Init();

    auto weaponSystem = gCoordinator.RegisterSystem<WeaponSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Weapon>());
        signature.set(gCoordinator.GetComponentType<Transform>());
        gCoordinator.SetSystemSignature<WeaponSystem>(signature);
    }
    weaponSystem->Init();

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
        signature.set(gCoordinator.GetComponentType<RigidBody>());
        gCoordinator.SetSystemSignature<PhysicsSystem>(signature);
    }
    physicsSystem->Init();

    const double                          fps = 60;
    std::chrono::system_clock::time_point start;

    // The server loop runs forever
    while (true) {
        start = std::chrono::system_clock::now();

        // Process packets.
        networkManager.processPackets();

        // Update game.
        waveSystem->Update();
        weaponSystem->Update();
        movementSystem->Update();
        physicsSystem->Update();

        const std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - start;
        const std::chrono::duration<double, std::milli> delta_ms((1000 / fps) - work_time.count());
        auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
    }

    // Close network.
    networkManager.close();
    threadNetworkLoop.join();

    // We call gCoordinator.Clear() before exiting the program, so as to make sure that destructors for the game objects
    // run before the SFML is destroyed at the global level
    gCoordinator.Clear();
}

int mainExceptionWrapped(int argc, char **argv)
{
    // Check arguments.
    if (argc < 2) {
        std::cout << "Try: " << argv[0] << " [port]." << std::endl;
        return (0);
    }

    // Start game server.
    GameLoop(std::atoi(argv[1]));

    return (0);
}

int main(int argc, char *argv[])
{
    try {
        return mainExceptionWrapped(argc, argv);
    } catch (std::exception &exc) {
        std::cerr << "Error (stdexcept): " << exc.what() << '\n';
        return 84;
    } catch (...) {
        std::cerr << "Error: Unknown exception !!!!!!\n";
        return 84;
    }
}
