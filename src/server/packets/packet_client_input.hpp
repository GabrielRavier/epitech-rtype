#pragma once

#include <bitset>
#include "packet.hpp"

enum InputType
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
    SHOOT,
};

class PacketClientInput : public Packet
{
public:
    std::bitset<8> inputs;

public:
    PacketClientInput(std::bitset<8> inputs) : inputs(inputs) {}
    PacketClientInput()     = default;
    ~PacketClientInput()    = default;

    void readPacket(Buffer *buffer)
    {
        this->inputs = buffer->readU8();
    };

    void writePacket(Buffer *buffer)
    {
        buffer->writeU8(static_cast<uint8_t>(this->inputs.to_ulong()));
    };

    void processPacket(INetworkHandler *handler);
};
