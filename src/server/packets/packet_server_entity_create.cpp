#include "packet_server_entity_create.hpp"
#include "../core/INetworkHandler.hpp"

void PacketServerEntityCreate::processPacket(INetworkHandler *handler)
{
    handler->processServerEntityCreate(this);
}
