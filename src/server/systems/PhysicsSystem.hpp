#pragma once

#include <memory>
#include "../../client/core/System.hpp"
#include "../../client/core/Coordinator.hpp"
#include "../../client/components/Player.hpp"
#include "../components/Network.hpp"
#include "../components/Projectile.hpp"
#include "../components/Transform.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Enemy.hpp"

class PhysicsSystem : public System
{
public:
    void        Init();
    void        Update() const;
    static bool Collided(const Transform &transformA, const RigidBody &bodyA, const Transform &transformB,
                         const RigidBody &bodyB);
};
