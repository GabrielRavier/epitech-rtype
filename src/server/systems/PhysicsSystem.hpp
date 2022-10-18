#pragma once

#include <memory>
#include "../../client/core/System.hpp"
#include "../../client/core/Coordinator.hpp"
#include "../../client/components/Player.hpp"
#include "../components/Network.hpp"
#include "../components/Projectile.hpp"
#include "../components/Transform.hpp"
#include "../components/RigidBody.hpp"

class PhysicsSystem : public System
{
public:
    void Init();
    void Update() const;
    bool Collided(Transform &transformA, RigidBody &bodyA, Transform &transformB, RigidBody &bodyB) const;
};
