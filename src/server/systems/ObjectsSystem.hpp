#pragma once

#include <memory>
#include "../../client/core/System.hpp"

class ObjectsSystem : public System
{
public:
    std::unordered_set<Entity> &GetEntities()
    {
        return mEntities;
    }
};
