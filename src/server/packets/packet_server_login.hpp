#pragma once

#include "packet.hpp"
#include "network_handler.hpp"

enum LoginState
{
    ACCEPT  = 0,
    DENY    = 1,
};

class PacketServerLogin : public Packet
{
public:
    LoginState state;

public:
    PacketServerLogin(LoginState state) : state(state) {}
    PacketServerLogin()     = default;
    ~PacketServerLogin()    = default;

    void readPacket(Buffer *buffer)
    {
        this->state = static_cast<LoginState>(buffer->readU8());
    };

    void writePacket(Buffer *buffer)
    {
        buffer->writeU8(this->state);
    };

    void processPacket(INetworkHandler *handler)
    {
        handler->processServerLogin(this);
    };
};
