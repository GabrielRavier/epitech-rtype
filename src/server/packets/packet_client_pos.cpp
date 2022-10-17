#include "packet_client_pos.hpp"
#include "../core/INetworkHandler.hpp"

void PacketClientPos::processPacket(INetworkHandler *handler)
{
    handler->processClientPos(this);
}
