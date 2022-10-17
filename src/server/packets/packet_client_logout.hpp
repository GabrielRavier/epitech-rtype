#pragma once

#include "packet.hpp"

class PacketClientLogout : public Packet
{
public:
    PacketClientLogout()           = default;
    ~PacketClientLogout() override = default;

    void readPacket(Buffer *buffer) override {}

    void writePacket(Buffer *buffer) override {}

    void processPacket(INetworkHandler *handler) override;
};
