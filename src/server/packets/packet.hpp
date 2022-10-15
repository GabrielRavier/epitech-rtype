#pragma once

#include "buffer.hpp"

class INetworkHandler;

class Packet
{
public:
    Packet() {};
    virtual ~Packet()                                       = default;
    virtual void readPacket(Buffer *buffer)                 = 0;
    virtual void writePacket(Buffer *buffer)                = 0;
    virtual void processPacket(INetworkHandler *handler)    = 0;
};
