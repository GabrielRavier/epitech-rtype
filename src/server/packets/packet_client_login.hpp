#pragma once

#include "packet.hpp"

class PacketClientLogin : public Packet
{
public:
    std::string username;

public:
    PacketClientLogin(std::string username) : username(username) {}
    PacketClientLogin()     = default;
    ~PacketClientLogin()    = default;

    void readPacket(Buffer buf)
    {
        this->username = buf.readString(4096);
    };

    void writePacket(Buffer buf)
    {
        buf.writeString(this->username);
    };
};
