#include "packet_server_update_score.hpp"
#include "../core/INetworkHandler.hpp"

void PacketServerUpdateScore::processPacket(INetworkHandler *handler)
{
    handler->processServerUpdateScore(this);
}
