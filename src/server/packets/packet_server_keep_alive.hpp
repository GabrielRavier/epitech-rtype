#pragma once

#include "packet.hpp"
#include "../utils.hpp"

class PacketServerKeepAlive: public Packet
{
public:
    uint64_t timestamp;

public:
    PacketServerKeepAlive() : timestamp(getCurrentTimeEpoch()) {}
    ~PacketServerKeepAlive() = default;

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
