#pragma once

#include "../core/Types.hpp"
#include <unordered_set>

class System
{
public:
    std::unordered_set<Entity> mEntities;
};
