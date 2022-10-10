#pragma once

#include "buffer.hpp"

class Packet
{
public:
    Packet() {};
    virtual ~Packet()                       = default;
    virtual void readPacket(Buffer buf)     = 0;
    virtual void writePacket(Buffer buf)    = 0;
};
