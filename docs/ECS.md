# The Entity Component System

The ECS (Entity Component System) is used in this project to represent essentially everything relating to the game's state.

This documentation assumes that you know the general concepts relating to an ECS. If you do not, reading articles such as [this](https://github.com/SanderMertens/ecs-faq), [this](https://web.archive.org/web/20171030021158/http://entity-systems-wiki.t-machine.org/), [this](https://gameprogrammingpatterns.com/component.html), [this](https://docs.unity3d.com/Packages/com.unity.entities@0.51/manual/index.html) or [this](https://www.sebaslab.com/entity-component-system-design-to-achieve-true-inversion-of-flow-control/) may help.

## The coordinator

This project defines a global variable called `gCoordinator`, of `Coordinator`. It contains all of the entities in the game, and is implemented using the `EntityManager`, `ComponentManager`, and `SystemManager` classes, which each manage the entities, components and systems of the game.

## Components

This project implements components as arbitrary data types that may contain any kind of data. To add a component, you just need to define a type containing various kinds of data you want to use with it, and then add a call to `gCoordinator.RegisterComponent<YourType>();` alongside all the other calls to `RegisterComponent` to make it usable by entities through `gCoordinator.AddComponent`.

## Systems

This project implements systems as types implementing the `System` interface. To add a system, you just need to define a class inheriting from `System`, i.e. like `class MyNewSystem : public System`. It is then registered with the coordinator using `gCoordinator.RegisterSystem<MyNewSystem>()`. The `System` interface provides an `mEntities` member that one can iterate over to act on certain entities. Which entities `mEntities` will contain depends on the signature of the system.
A signature is represented as a bitset of component types that one wants to use with a given system. Component types are obtained using `gCoordinator.GetComponentType<SomeComponent>()`, and a complete signature is then set as the signature for a given system using `gCoordinator.SetSystemSignature<SomeSystem>(theSignatureFilledWithTheWantedComponentTypes)`. As a complete example, a signature might be setup as such:

```cpp
Signature signature;
signature.set(gCoordinator.GetComponentType<SomeComponent>());
signature.set(gCoordinator.GetComponentType<SomeOtherComponent>());
gCoordinator.SetSystemSignature<SomeSystem>(signature);
```

## Entities

This project implements entities as 16-bit integers, as defined in [Types.hpp](../src/client/core/Types.hpp).

An entity can be created using `Coordinator::CreateEntity`, and can be destroyed using `Coordinator::DestroyEntity`
Components can be added to an entity using `Coordinator::AddComponent`, and removed using `Coordinator::RemoveComponent`
