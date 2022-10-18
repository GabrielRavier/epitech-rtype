#pragma once

#include <bitset>

class NetworkClientManager;

struct Network
{
    NetworkClientManager    *manager;
    std::bitset<8>          inputs;
};
