#pragma once

#include "packet.hpp"

class PacketClientPos : public Packet
{
public:
    int16_t posX;
    int16_t posY;

public:
    PacketClientPos(int16_t posX, int16_t posY) : posX(posX), posY(posY) {}

    PacketClientPos()           = default;
    ~PacketClientPos() override = default;

    void readPacket(Buffer *buffer) override
    {
        this->posX = static_cast<int16_t>(buffer->readU16());
        this->posY = static_cast<int16_t>(buffer->readU16());
    };

    void writePacket(Buffer *buffer) override
    {
        buffer->writeU16(this->posX);
        buffer->writeU16(this->posY);
    };

    void processPacket(INetworkHandler *handler) override;
};
