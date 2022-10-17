#pragma once

#include "packet.hpp"

class PacketClientPos : public Packet
{
public:
    uint16_t posX;
    uint16_t posY;

public:
    PacketClientPos(uint16_t posX, uint16_t posY) : posX(posX), posY(posY) {}
    PacketClientPos()     = default;
    ~PacketClientPos()    = default;

    void readPacket(Buffer *buffer)
    {
        this->posX = buffer->readU16();
        this->posY = buffer->readU16();
    };

    void writePacket(Buffer *buffer)
    {
        buffer->writeU16(this->posX);
        buffer->writeU16(this->posY);
    };

    void processPacket(INetworkHandler *handler);
};
