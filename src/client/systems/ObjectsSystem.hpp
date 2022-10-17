#pragma once

#include "../core/System.hpp"
#include "../core/Coordinator.hpp"
#include "../components/Movement.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Transform.hpp"

class ObjectsSystem : public System
{
public:
    void   Init(Entity me);
    Entity GetEntityByNetworkId(Entity networkEntityId);
    Entity GetMe() const;

private:
    Entity _me;
    float  _xLimit;
    float  _yLimit;
};
