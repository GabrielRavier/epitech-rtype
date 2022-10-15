#pragma once

#include "packet.hpp"
#include "network_handler.hpp"

class PacketClientLogin : public Packet
{
public:
    std::string username;

public:
    PacketClientLogin(std::string username) : username(username) {}
    PacketClientLogin()     = default;
    ~PacketClientLogin()    = default;

    void readPacket(Buffer *buffer)
    {
        this->username = buffer->readString(4096);
    };

    void writePacket(Buffer *buffer)
    {
        buffer->writeString(this->username);
    };

    void processPacket(INetworkHandler *handler)
    {
        handler->processClientLogin(this);
    };
};
