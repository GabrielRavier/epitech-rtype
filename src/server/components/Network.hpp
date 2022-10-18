#pragma once

#include <bitset>
#include <cstdint>

class NetworkClientManager;

struct Network
{
    NetworkClientManager *manager;
    std::bitset<8>        inputs;
    uint16_t              timeout;
};
