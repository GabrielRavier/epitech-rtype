#include "core/Coordinator.hpp"
#include "components/Player.hpp"
#include "components/RigidBody.hpp"
#include "components/Transform.hpp"
#include <chrono>

Coordinator gCoordinator;

int main()
{
    printf("Starting client\n");
    gCoordinator.RegisterComponent<Player>();
    gCoordinator.RegisterComponent<RigidBody>();
    gCoordinator.RegisterComponent<Transform>();

    // float dt = 0.0f;
    int temp = 0;

    while (temp < 10000) {
        // auto startTime = std::chrono::high_resolution_clock::now();

        // physicsSystem->Update(dt);

        // auto stopTime = std::chrono::high_resolution_clock::now();
        // dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
        temp += 1;
    }

    return (0);
}
