#include "packet_client_keep_alive.hpp"
#include "network_handler.hpp"

void PacketClientKeepAlive::processPacket(INetworkHandler *handler)
{
    handler->processClientKeepAlive(this);
};
