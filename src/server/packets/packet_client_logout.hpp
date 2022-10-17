#pragma once

#include "packet.hpp"

class PacketClientLogout: public Packet
{
public:
    PacketClientLogout()    = default;
    ~PacketClientLogout()   = default;

    void readPacket(Buffer *buffer) {}
    void writePacket(Buffer *buffer) {}
    void processPacket(INetworkHandler *handler);
};
