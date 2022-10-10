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

    void readPacket(Buffer buf)
    {
        this->entityId  = buf.readU16();
        this->life      = buf.readU16();
    };

    void writePacket(Buffer buf)
    {
        buf.writeU16(this->entityId);
        buf.writeU16(this->life);
    };
};
