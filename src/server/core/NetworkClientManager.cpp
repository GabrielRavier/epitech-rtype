#include "NetworkClientManager.hpp"
#include "NetworkServerManager.hpp"

void NetworkClientManager::send(Packet *packet)
{
    Buffer *buffer = new Buffer(4096);
    size_t packetSize;

    // Encode packet to buffer.
    buffer->setPos(2);
    buffer->writeU16(0);
    buffer->writeU16(0);
    buffer->writeU8(GetServerPacketId(packet));
    packet->writePacket(buffer);
    packetSize = buffer->pos();
    buffer->setPos(0);
    buffer->writeU16(static_cast<uint16_t>(packetSize - 2));
    buffer->setPos(packetSize);

    // Send packet.
    m_server->socket().send_to(boost::asio::buffer(buffer->data(), packetSize), m_remote_endpoint);

    // Delete buffer.
    delete buffer;
}

void NetworkClientManager::processClientLogin(PacketClientLogin *packet)
{

}

void NetworkClientManager::processClientKeepAlive(PacketClientKeepAlive *packet)
{

}

void NetworkClientManager::processClientInput(PacketClientInput *packet)
{
    std::cerr << packet->inputs.to_string() << std::endl;
}
