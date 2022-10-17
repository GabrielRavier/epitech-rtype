#pragma once

#include <unordered_set>
#include "../core/Types.hpp"

class System
{
public:
    std::unordered_set<Entity> mEntities;
};
