#pragma once

#include "packet.hpp"
#include "../utils.hpp"

class PacketClientKeepAlive : public Packet
{
public:
    uint64_t timestamp;

public:
    PacketClientKeepAlive() : timestamp(getCurrentTimeEpoch()) {}

    ~PacketClientKeepAlive() override = default;

    void readPacket(Buffer *buffer) override
    {
        this->timestamp = buffer->readU64();
    };

    void writePacket(Buffer *buffer) override
    {
        buffer->writeU64(this->timestamp);
    };

    void processPacket(INetworkHandler *handler) override;
};
