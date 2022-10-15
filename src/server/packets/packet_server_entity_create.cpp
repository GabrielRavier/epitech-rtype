#include "packet_server_entity_create.hpp"
#include "network_handler.hpp"

void PacketServerEntityCreate::processPacket(INetworkHandler *handler)
{
    handler->processServerEntityCreate(this);
}
