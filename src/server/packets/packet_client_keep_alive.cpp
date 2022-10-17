#include "packet_client_keep_alive.hpp"
#include "../core/INetworkHandler.hpp"

void PacketClientKeepAlive::processPacket(INetworkHandler *handler)
{
    handler->processClientKeepAlive(this);
};
