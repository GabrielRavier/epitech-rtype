#pragma once

#include <string>
#include <vector>
#include "../core/Types.hpp"

struct Player
{
    std::string         name;
    int                 health;
    int                 power;
    std::vector<Entity> weapons;
};
