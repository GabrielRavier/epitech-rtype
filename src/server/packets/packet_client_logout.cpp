#include "packet_client_logout.hpp"
#include "../core/INetworkHandler.hpp"

void PacketClientLogout::processPacket(INetworkHandler *handler)
{
    handler->processClientLogout(this);
}
