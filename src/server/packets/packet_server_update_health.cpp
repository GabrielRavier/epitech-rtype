#include "packet_server_update_health.hpp"
#include "network_handler.hpp"

void PacketServerUpdateHealth::processPacket(INetworkHandler *handler)
{
    handler->processServerUpdateHealth(this);
}
