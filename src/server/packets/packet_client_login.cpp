#include "packet_client_login.hpp"
#include "../core/INetworkHandler.hpp"

void PacketClientLogin::processPacket(INetworkHandler *handler)
{
    handler->processClientLogin(this);
}
