#include "packet_server_entity_destroy.hpp"
#include "../core/INetworkHandler.hpp"

void PacketServerEntityDestroy::processPacket(INetworkHandler *handler)
{
    handler->processServerEntityDestroy(this);
}
