#pragma once

#include "packet.hpp"
#include "network_handler.hpp"

class PacketServerUpdateScore : public Packet
{
public:
    uint64_t score;

public:
    PacketServerUpdateScore(uint64_t score) : score(score) {}
    PacketServerUpdateScore()  = default;
    ~PacketServerUpdateScore() = default;

    void readPacket(Buffer *buffer)
    {
        this->score = buffer->readU64();
    };

    void writePacket(Buffer *buffer)
    {
        buffer->writeU64(this->score);
    };

    void processPacket(INetworkHandler *handler)
    {
        handler->processServerUpdateScore(this);
    };
};
