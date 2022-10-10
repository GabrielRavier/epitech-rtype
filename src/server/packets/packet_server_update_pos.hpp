#pragma once

#include "packet.hpp"

class PacketServerUpdatePos : public Packet
{
public:
    uint16_t entityId;
    uint16_t posX;
    uint16_t posY;

public:
    PacketServerUpdatePos(uint16_t entityId, uint16_t posX, uint16_t posY) :
        entityId(entityId),
        posX(posX),
        posY(posY) {}
    PacketServerUpdatePos()     = default;
    ~PacketServerUpdatePos()    = default;

    void readPacket(Buffer buf)
    {
        this->entityId      = buf.readU16();
        this->posX          = buf.readU16();
        this->posY          = buf.readU16();
    };

    void writePacket(Buffer buf)
    {
        buf.writeU16(this->entityId);
        buf.writeU16(this->posX);
        buf.writeU16(this->posY);
    };
};
