#include "packet_server_update_health.hpp"
#include "../core/INetworkHandler.hpp"

void PacketServerUpdateHealth::processPacket(INetworkHandler *handler)
{
    handler->processServerUpdateHealth(this);
}
