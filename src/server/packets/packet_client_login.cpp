#include "packet_client_login.hpp"
#include "network_handler.hpp"

void PacketClientLogin::processPacket(INetworkHandler *handler)
{
    handler->processClientLogin(this);
}
