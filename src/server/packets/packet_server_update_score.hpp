#pragma once

#include "packet.hpp"

class PacketServerUpdateScore : public Packet
{
public:
    uint64_t score;

public:
    PacketServerUpdateScore(uint64_t score) : score(score) {}
    PacketServerUpdateScore()  = default;
    ~PacketServerUpdateScore() = default;

    void readPacket(Buffer buf)
    {
        this->score = buf.readU64();
    };

    void writePacket(Buffer buf)
    {
        buf.writeU64(this->score);
    };
};
