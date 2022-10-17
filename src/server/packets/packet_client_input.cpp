#include "packet_client_input.hpp"
#include "../core/INetworkHandler.hpp"

void PacketClientInput::processPacket(INetworkHandler *handler)
{
    handler->processClientInput(this);
}
