#pragma once

#include "packet.hpp"

class PacketServerUpdateHealth : public Packet
{
public:
    uint16_t entityId;
    uint16_t life;

public:
    PacketServerUpdateHealth(uint16_t entityId, uint16_t life) : entityId(entityId), life(life) {}
    PacketServerUpdateHealth()  = default;
    ~PacketServerUpdateHealth() = default;

    void readPacket(Buffer *buffer)
    {
        this->entityId  = buffer->readU16();
        this->life      = buffer->readU16();
    };

    void writePacket(Buffer *buffer)
    {
        buffer->writeU16(this->entityId);
        buffer->writeU16(this->life);
    };

    void processPacket(INetworkHandler *handler);
};
