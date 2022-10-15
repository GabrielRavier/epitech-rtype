#include "packet_server_update_score.hpp"
#include "network_handler.hpp"

void PacketServerUpdateScore::processPacket(INetworkHandler *handler)
{
    handler->processServerUpdateScore(this);
}
