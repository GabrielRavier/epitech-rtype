#pragma once

#include "packet.hpp"
#include "network_handler.hpp"

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

    void readPacket(Buffer *buffer)
    {
        this->entityId      = buffer->readU16();
        this->posX          = buffer->readU16();
        this->posY          = buffer->readU16();
    };

    void writePacket(Buffer *buffer)
    {
        buffer->writeU16(this->entityId);
        buffer->writeU16(this->posX);
        buffer->writeU16(this->posY);
    };

    void processPacket(INetworkHandler *handler)
    {
        handler->processServerUpdatePos(this);
    };
};
