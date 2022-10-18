#pragma once

#include "packet.hpp"

class PacketClientPos : public Packet
{
public:
    uint16_t posX;
    uint16_t posY;

public:
    PacketClientPos(uint16_t posX, uint16_t posY) : posX(posX), posY(posY) {}

    PacketClientPos()           = default;
    ~PacketClientPos() override = default;

    void readPacket(Buffer *buffer) override
    {
        this->posX = buffer->readU16();
        this->posY = buffer->readU16();
    };

    void writePacket(Buffer *buffer) override
    {
        buffer->writeU16(this->posX);
        buffer->writeU16(this->posY);
    };

    void processPacket(INetworkHandler *handler) override;
};