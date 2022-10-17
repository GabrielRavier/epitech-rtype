#pragma once

#include "packet.hpp"

class PacketClientLogin : public Packet
{
public:
    std::string username;

public:
    explicit PacketClientLogin(std::string username) : username(std::move(username)) {}

    PacketClientLogin()           = default;
    ~PacketClientLogin() override = default;

    void readPacket(Buffer *buffer) override
    {
        this->username = buffer->readString(4096);
    };

    void writePacket(Buffer *buffer) override
    {
        buffer->writeString(this->username);
    };

    void processPacket(INetworkHandler *handler) override;
};
