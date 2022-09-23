#include "core/Coordinator.hpp"
#include "components/Player.hpp"
#include "components/RigidBody.hpp"
#include "components/Sprite.hpp"
#include "components/Transform.hpp"
#include "components/Window.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/WindowSystem.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>

Coordinator gCoordinator;

int main()
{
    printf("Starting client\n");

    gCoordinator.RegisterComponent<Player>();
    gCoordinator.RegisterComponent<RigidBody>();
    gCoordinator.RegisterComponent<Sprite>();
    gCoordinator.RegisterComponent<Transform>();
    gCoordinator.RegisterComponent<Window>();

    /*auto entity = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(
        entity,
        Transform{
            .position = Vec2(0, 0),
            .scale = Vec2(0, 0),
            .rotation = 0
        });

    auto windowSystem = gCoordinator.RegisterSystem<WindowSystem>();
    {
		Signature signature;
		signature.set(gCoordinator.GetComponentType<Window>());
		gCoordinator.SetSystemSignature<WindowSystem>(signature);
	}
    windowSystem->Init();
    
    auto renderSystem = gCoordinator.RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<Window>());
        signature.set(gCoordinator.GetComponentType<Sprite>());
        signature.set(gCoordinator.GetComponentType<Transform>());
        gCoordinator.SetSystemSignature<RenderSystem>(signature);
    }
    renderSystem->Init();*/

    float dt = 0.0f;

    while (true) {
        auto startTime = std::chrono::high_resolution_clock::now();
        //renderSystem->Update(dt);
        windowSystem->Update();
        auto stopTime = std::chrono::high_resolution_clock::now();
        dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
    }

    //windowSystem->Shutdown();
    return (0);
}
