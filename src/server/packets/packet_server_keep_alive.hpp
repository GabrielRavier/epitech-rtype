#pragma once

#include "packet.hpp"
#include "../utils.hpp"

class PacketServerKeepAlive : public Packet
{
public:
    uint64_t timestamp;

public:
    PacketServerKeepAlive() : timestamp(getCurrentTimeEpoch()) {}

    ~PacketServerKeepAlive() override = default;

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
