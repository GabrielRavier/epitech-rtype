#pragma once

#include <cstdint>
#include "../packets/packet_server_entity_create.hpp"

struct Transform
{
    EntityType type;
    uint16_t   posX;
    uint16_t   posY;
};
