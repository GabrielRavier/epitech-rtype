#include "packet_server_login.hpp"
#include "../core/INetworkHandler.hpp"

void PacketServerLogin::processPacket(INetworkHandler *handler)
{
    handler->processServerLogin(this);
}
