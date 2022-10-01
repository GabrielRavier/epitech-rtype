#include "../client/core/Coordinator.hpp"
#include <cassert>

struct TestComponent
{
    int x;
};

struct TestSystem : System
{};

Coordinator gCoordinator;

int main()
{
    gCoordinator.RegisterComponent<TestComponent>();
    auto testSystem = gCoordinator.RegisterSystem<TestSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<TestComponent>());
        gCoordinator.SetSystemSignature<TestSystem>(signature);
    }

    auto entity1 = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(entity1, TestComponent{124});

    gCoordinator.CreateEntity(); // This is to create a dummy entity that we check that we've skipped over
    auto entity3 = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(entity3, TestComponent{421});

    auto foundEntities = testSystem->mEntities.begin();
    if (gCoordinator.GetComponent<TestComponent>(*foundEntities++).x != 421)
        std::abort();
    if (gCoordinator.GetComponent<TestComponent>(*foundEntities++).x != 124)
        std::abort();
    if (foundEntities != testSystem->mEntities.end())
        std::abort();
}
