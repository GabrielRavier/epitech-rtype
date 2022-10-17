#pragma once

#include "../../client/core/Types.hpp"
#include "packet.hpp"

enum LoginState
{
    ACCEPT = 0,
    DENY   = 1,
};

class PacketServerLogin : public Packet
{
public:
    LoginState state;
    Entity     entityId;

public:
    PacketServerLogin(LoginState state, Entity entityId) : state(state), entityId(entityId) {}

    PacketServerLogin()           = default;
    ~PacketServerLogin() override = default;

    void readPacket(Buffer *buffer) override
    {
        this->state    = static_cast<LoginState>(buffer->readU8());
        this->entityId = buffer->readU16();
    };

    void writePacket(Buffer *buffer) override
    {
        buffer->writeU8(this->state);
        buffer->writeU16(this->entityId);
    };

    void processPacket(INetworkHandler *handler) override;
};
