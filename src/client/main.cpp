#include "core/Coordinator.hpp"
#include "core/WindowManager.hpp"
#include "components/Movement.hpp"
#include "components/Player.hpp"
#include "components/RigidBody.hpp"
#include "components/Sprite.hpp"
#include "components/Transform.hpp"
#include "systems/BackgroundSystem.hpp"
#include "systems/PhysicsSystem.hpp"
#include "systems/PlayerSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/MovementSystem.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <loguru.hpp>
#include <ctime>
#include <chrono>
#include <thread>

Coordinator gCoordinator;

int main(int argc, char *argv[])
{
    loguru::init(argc, argv);
    loguru::add_file("r-type_client.log", loguru::Append, loguru::Verbosity_MAX);
    LOG_F(INFO, "Starting client");

    const std::shared_ptr<WindowManager> windowManager = std::make_shared<WindowManager>();
    windowManager->Init("R-Type", 1920, 700);

    gCoordinator.RegisterComponent<Movement>();
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
    return (0);
}
