#pragma once

#include "packet.hpp"
#include "../utils.hpp"

class PacketClientKeepAlive: public Packet
{
public:
    uint64_t timestamp;

public:
    PacketClientKeepAlive() : timestamp(getCurrentTimeEpoch()) {}
    ~PacketClientKeepAlive() = default;

    void readPacket(Buffer *buffer)
    {
        this->timestamp = buffer->readU64();
    };

    void writePacket(Buffer *buffer)
    {
        buffer->writeU64(this->timestamp);
    };

    void processPacket(INetworkHandler *handler);
};
