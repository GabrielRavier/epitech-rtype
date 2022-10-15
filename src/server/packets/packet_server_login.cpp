#include "packet_server_login.hpp"
#include "network_handler.hpp"

void PacketServerLogin::processPacket(INetworkHandler *handler)
{
    handler->processServerLogin(this);
}
