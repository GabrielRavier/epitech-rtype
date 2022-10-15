#include "packet_server_keep_alive.hpp"
#include "network_handler.hpp"

void PacketServerKeepAlive::processPacket(INetworkHandler *handler)
{
    handler->processServerKeepAlive(this);
}
