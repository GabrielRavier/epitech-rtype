#pragma once

#include <cstdint>
#include "../packets/packet_server_entity_create.hpp"

struct Transform
{
    EntityType type;
    int16_t    posX;
    int16_t    posY;
};
