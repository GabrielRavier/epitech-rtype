#pragma once

#include <chrono>
#include "packet.hpp"

uint64_t timeSinceEpochMillisec()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

class PacketClientKeepAlive: public Packet
{
public:
    uint64_t timestamp;

public:
    PacketClientKeepAlive() : timestamp(timeSinceEpochMillisec()) {}
    ~PacketClientKeepAlive() = default;

    void readPacket(Buffer buf)
    {
        this->timestamp = buf.readU64();
    };

    void writePacket(Buffer buf)
    {
        buf.writeU64(this->timestamp);
    };
};
