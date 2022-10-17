#include "packet_server_update_pos.hpp"
#include "../core/INetworkHandler.hpp"

void PacketServerUpdatePos::processPacket(INetworkHandler *handler)
{
    handler->processServerUpdatePos(this);
}
