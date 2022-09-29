#include "core/Coordinator.hpp"
#include "core/WindowManager.hpp"
#include "components/Movement.hpp"
#include "components/Player.hpp"
#include "components/RigidBody.hpp"
#include "components/Sprite.hpp"
#include "components/Transform.hpp"
#include "systems/BackgroundSystem.hpp"
#include "systems/PlayerSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/MovementSystem.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>

Coordinator gCoordinator;

int main()
{
    printf("Starting client\n");

    std::shared_ptr<WindowManager> windowManager = std::make_shared<WindowManager>();
    windowManager->Init("R-Type", 1920, 1080);

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

    auto playerSystem = gCoordinator.RegisterSystem<PlayerSystem>();
    {
        Signature signature;
        gCoordinator.SetSystemSignature<PlayerSystem>(signature);
    }
    playerSystem->Init();

    auto backgroundSystem = gCoordinator.RegisterSystem<BackgroundSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Transform>());
        gCoordinator.SetSystemSignature<BackgroundSystem>(signature);
    }
    backgroundSystem->Init();

    float delta = 0.0f;
    auto  last  = std::chrono::high_resolution_clock::now();

    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        delta += std::chrono::duration<float, std::chrono::seconds::period>(now - last).count() * 1000;
        last = now;
        while (delta >= 1) {
            windowManager->Clear();
            movementSystem->Update();
            backgroundSystem->Update();
            renderSystem->Update(windowManager);
            windowManager->Update();
            delta -= 1;
        }
    }
    return (0);
}
