#pragma once

#include "packet.hpp"
#include "../../client/core/Types.hpp"

class PacketServerEntityDestroy : public Packet
{
public:
    Entity entityId;

public:
    explicit PacketServerEntityDestroy(Entity entityId) : entityId(entityId) {}

    PacketServerEntityDestroy()           = default;
    ~PacketServerEntityDestroy() override = default;

    void readPacket(Buffer *buffer) override
    {
        this->entityId = buffer->readU16();
    }

    void writePacket(Buffer *buffer) override
    {
        buffer->writeU16(this->entityId);
    }

    void processPacket(INetworkHandler *handler) override;
};
