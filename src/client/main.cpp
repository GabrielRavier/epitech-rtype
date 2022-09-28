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
#include <ctime>

Coordinator gCoordinator;

int main()
{
    printf("Starting client\n");

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

    bool       running = true;
    const long temp    = (1000l / 60l) * 1000000l;
    timespec   oldtime;

    while (running) {
        clock_gettime(CLOCK_REALTIME, &oldtime);
        oldtime.tv_nsec += temp;
        if (oldtime.tv_nsec > 999999999) {
            oldtime.tv_nsec -= 1000000000;
            oldtime.tv_sec++;
        }
        running = windowManager->ManageEvent();
        playerSystem->Update(windowManager->GetPressedButtons());
        backgroundSystem->Update();
        movementSystem->Update();
        physicsSystem->Update();
        renderSystem->Update(windowManager);
        windowManager->Update();
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &oldtime, nullptr);
    }
    return (0);
}
