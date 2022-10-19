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
#include "systems/MainMenuSystem.hpp"
#include "systems/MovementSystem.hpp"
#include "systems/ObjectsSystem.hpp"
#include "core/SceneManager.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <loguru.hpp>
#include <ctime>
#include <chrono>
#include <thread>

Coordinator                    gCoordinator;
std::shared_ptr<ObjectsSystem> gObjectsSystem;
NetworkManager                *gNetworkManager;

int mainExceptionWrapped(int argc, char *argv[])
{
    loguru::init(argc, argv);
    loguru::add_file("r-type_client.log", loguru::Append, loguru::Verbosity_MAX);
    LOG_F(INFO, "Starting client");

    // Check arguments.
    if (argc < 3) {
        std::cout << "Try: " << argv[0] << " [host] [port]." << std::endl;
        return (0);
    }

    std::unique_ptr<SceneManager> sceneManager = std::make_unique<SceneManager>();
    sceneManager->Init(argv[1], std::atoi(argv[2]));
    sceneManager->Loop();

    sceneManager->Clear();
    gCoordinator.Clear();
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
