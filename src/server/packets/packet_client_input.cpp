#include "packet_client_input.hpp"
#include "network_handler.hpp"

void PacketClientInput::processPacket(INetworkHandler *handler)
{
    handler->processClientInput(this);
}
