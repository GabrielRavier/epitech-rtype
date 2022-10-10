#pragma once

#include <bitset>
#include "packet.hpp"

enum InputType
{
    LEFT    = 1 << 0,
    TOP     = 1 << 1,
    RIGHT   = 1 << 2,
    BOTTOM  = 1 << 3,
};

class PacketClientInput : public Packet
{
public:
    std::bitset<8> inputs;

public:
    PacketClientInput(std::bitset<8> inputs) : inputs(inputs) {}
    PacketClientInput()     = default;
    ~PacketClientInput()    = default;

    void readPacket(Buffer buf)
    {
        this->inputs = buf.readU8();
    };

    void writePacket(Buffer buf)
    {
        buf.writeU8(this->inputs.to_ulong());
    };
};
