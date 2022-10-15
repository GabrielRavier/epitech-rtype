#include "packet_server_update_pos.hpp"
#include "network_handler.hpp"

void PacketServerUpdatePos::processPacket(INetworkHandler *handler)
{
    handler->processServerUpdatePos(this);
}
