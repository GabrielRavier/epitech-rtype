#pragma once

#include <cstdint>
#include "packet.hpp"

uint8_t GetClientPacketId(Packet *packet);
uint8_t GetServerPacketId(Packet *packet);

Packet *CreateClientPacket(uint8_t id);
Packet *CreateServerPacket(uint8_t id);
