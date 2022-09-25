#include "core/Coordinator.hpp"
#include "core/WindowManager.hpp"
#include "components/Player.hpp"
#include "components/RigidBody.hpp"
#include "components/Sprite.hpp"
#include "components/Transform.hpp"
#include "systems/BackgroundSystem.hpp"
#include "systems/RenderSystem.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>

Coordinator gCoordinator;

int main()
{
    printf("Starting client\n");

    std::shared_ptr<WindowManager> windowManager = std::make_shared<WindowManager>();
    windowManager->Init("R-Type", 1080, 720);

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

    auto backgroundSystem = gCoordinator.RegisterSystem<BackgroundSystem>();
    {
		Signature signature;
        signature.set(gCoordinator.GetComponentType<Transform>());
		gCoordinator.SetSystemSignature<BackgroundSystem>(signature);
	}
    backgroundSystem->Init();

    float dt = 0.0f;

    while (true) {
        auto startTime = std::chrono::high_resolution_clock::now();
        windowManager->Clear();
        backgroundSystem->Update(dt);
        renderSystem->Update(dt, windowManager);
        windowManager->Update();
        auto stopTime = std::chrono::high_resolution_clock::now();
        dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
    }
    return (0);
}
