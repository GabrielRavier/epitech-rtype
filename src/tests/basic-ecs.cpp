#include "../client/core/Coordinator.hpp"

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

    // The order is unpredicable, as mEntities is an unordered_set
    int x1 = gCoordinator.GetComponent<TestComponent>(*foundEntities++).x;
    int x2 = gCoordinator.GetComponent<TestComponent>(*foundEntities++).x;
    if (!((x1 == 421 && x2 == 124) || (x1 == 124 && x2 == 421)))
        std::abort();
    if (foundEntities != testSystem->mEntities.end())
        std::abort();
}
