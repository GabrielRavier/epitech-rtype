#pragma once

#include "../core/System.hpp"
#include "../core/Coordinator.hpp"
#include "../components/Movement.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Transform.hpp"

class PhysicsSystem : public System
{
public:
    void Init(float windowWidth, float windowHeight);
    void Update();

private:
    float _xLimit;
    float _yLimit;
};
