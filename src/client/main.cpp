#include "core/Coordinator.hpp"
#include "core/WindowManager.hpp"
#include "components/Enemy.hpp"
#include "components/Movement.hpp"
#include "components/Player.hpp"
#include "components/RigidBody.hpp"
#include "components/Sprite.hpp"
#include "components/Transform.hpp"
#include "components/Weapon.hpp"
#include "systems/BackgroundSystem.hpp"
#include "systems/PhysicsSystem.hpp"
#include "systems/PlayerSystem.hpp"
#include "systems/ProjectileSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/MovementSystem.hpp"
#include "systems/WaveSystem.hpp"
#include "systems/WeaponSystem.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <loguru.hpp>
#include <ctime>
#include <chrono>
#include <thread>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "../server/sync_queue.hpp"
#include "../server/packets/packet.hpp"

boost::asio::ip::udp::socket *_socket;
SynchronisedQueue<Packet> queue;
Coordinator gCoordinator;

void NetworkLoop(const char *host, uint16_t port)
{
    try {
        boost::asio::io_service io_service;
        boost::asio::ip::udp::resolver resolver(io_service);
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host, "");
        boost::asio::ip::udp::endpoint target_endpoint = *resolver.resolve(query);
        boost::asio::ip::udp::endpoint sender_endpoint;

        _socket = new boost::asio::ip::udp::socket(io_service);
        _socket->open(boost::asio::ip::udp::v4());

        // Set port of connection.
        target_endpoint.port(port);

        boost::array<char, 1> send_buf = { 10 };
        _socket->send_to(boost::asio::buffer(send_buf), target_endpoint);

        while (_socket->is_open()) {
            char *packet = new char[199];
            _socket->receive_from(boost::asio::buffer(packet, 199), sender_endpoint);
            std::string address = sender_endpoint.address().to_string() + std::to_string(sender_endpoint.port());
            std::cout << "TEST." << std::endl << address << std::endl;
        }

    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void GameLoop(const char *host, uint16_t port)
{
    std::thread threadNetworkLoop(NetworkLoop, host, port);

    const std::shared_ptr<WindowManager> windowManager = std::make_shared<WindowManager>();
    windowManager->Init("R-Type", 1920, 700);

    gCoordinator.RegisterComponent<Enemy>();
    gCoordinator.RegisterComponent<Movement>();
    gCoordinator.RegisterComponent<Player>();
    gCoordinator.RegisterComponent<Projectile>();
    gCoordinator.RegisterComponent<RigidBody>();
    gCoordinator.RegisterComponent<Sprite>();
    gCoordinator.RegisterComponent<Transform>();
    gCoordinator.RegisterComponent<Weapon>();

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

    auto weaponSystem = gCoordinator.RegisterSystem<WeaponSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Weapon>());
        signature.set(gCoordinator.GetComponentType<Transform>());
        gCoordinator.SetSystemSignature<WeaponSystem>(signature);
    }
    weaponSystem->Init();

    auto physicsSystem = gCoordinator.RegisterSystem<PhysicsSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Movement>());
        signature.set(gCoordinator.GetComponentType<Transform>());
        signature.set(gCoordinator.GetComponentType<RigidBody>());
        gCoordinator.SetSystemSignature<PhysicsSystem>(signature);
    }
    physicsSystem->Init(1920, 700);

    auto waveSystem = gCoordinator.RegisterSystem<WaveSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Enemy>());
        signature.set(gCoordinator.GetComponentType<Weapon>());
        gCoordinator.SetSystemSignature<WaveSystem>(signature);
    }
    waveSystem->Init();

    auto projectileSystem = gCoordinator.RegisterSystem<ProjectileSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Projectile>());
        gCoordinator.SetSystemSignature<ProjectileSystem>(signature);
    }
    projectileSystem->Init();

    auto playerSystem = gCoordinator.RegisterSystem<PlayerSystem>();
    playerSystem->Init();

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
        running = windowManager->ManageEvent();
        playerSystem->Update(windowManager->GetPressedButtons());
        waveSystem->Update();
        weaponSystem->Update();
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

    // Close socket and wait network thread stop.
    _socket->close();
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

    // Start network and game.
    GameLoop(argv[1], std::atoi(argv[2]));

    return (0);
}
