#include "packet_server_keep_alive.hpp"
#include "../core/INetworkHandler.hpp"

void PacketServerKeepAlive::processPacket(INetworkHandler *handler)
{
    handler->processServerKeepAlive(this);
}
