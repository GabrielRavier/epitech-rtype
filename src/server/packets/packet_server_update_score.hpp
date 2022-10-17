#pragma once

#include "packet.hpp"

class PacketServerUpdateScore : public Packet
{
public:
    uint64_t score;

public:
    explicit PacketServerUpdateScore(uint64_t score) : score(score) {}

    PacketServerUpdateScore()           = default;
    ~PacketServerUpdateScore() override = default;

    void readPacket(Buffer *buffer) override
    {
        this->score = buffer->readU64();
    };

    void writePacket(Buffer *buffer) override
    {
        buffer->writeU64(this->score);
    };

    void processPacket(INetworkHandler *handler) override;
};
