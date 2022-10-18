#pragma once

#include "packet.hpp"

class PacketServerUpdatePos : public Packet
{
public:
    uint16_t entityId;
    int16_t  posX;
    int16_t  posY;

public:
    PacketServerUpdatePos(uint16_t entityId, int16_t posX, int16_t posY) : entityId(entityId), posX(posX), posY(posY) {}

    PacketServerUpdatePos()           = default;
    ~PacketServerUpdatePos() override = default;

    void readPacket(Buffer *buffer) override
    {
        this->entityId = buffer->readU16();
        this->posX     = static_cast<int16_t>(buffer->readU16());
        this->posY     = static_cast<int16_t>(buffer->readU16());
    };

    void writePacket(Buffer *buffer) override
    {
        buffer->writeU16(this->entityId);
        buffer->writeU16(this->posX);
        buffer->writeU16(this->posY);
    };

    void processPacket(INetworkHandler *handler) override;
};
