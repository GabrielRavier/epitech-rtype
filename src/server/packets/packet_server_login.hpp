#pragma once

#include "packet.hpp"

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

    void readPacket(Buffer buf)
    {
        this->state = static_cast<LoginState>(buf.readU8());
    };

    void writePacket(Buffer buf)
    {
        buf.writeU8(this->state);
    };
};
