#pragma once

#include "packet.hpp"
#include "../../client/core/Types.hpp"

class PacketServerEntityDestroy: public Packet
{
public:
    Entity entityId;

public:
    PacketServerEntityDestroy(Entity entityId) : entityId(entityId) {}
    PacketServerEntityDestroy()     = default;
    ~PacketServerEntityDestroy()    = default;

    void readPacket(Buffer *buffer)
    {
        this->entityId = buffer->readU16();
    }

    void writePacket(Buffer *buffer)
    {
        buffer->writeU16(this->entityId);
    }

    void processPacket(INetworkHandler *handler);
};
