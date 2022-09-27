#pragma once

#include "../core/System.hpp"
#include "../core/Coordinator.hpp"

class PhysicsSystem : public System
{
public:
    void Init();
    void Update();
};
