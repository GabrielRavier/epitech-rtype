#include "NetworkServerManager.hpp"

void NetworkServerManager::run()
{
    Buffer                         tmp_buffer(4096);
    boost::asio::ip::udp::endpoint remote_endpoint;

    while (m_connection.isOpen()) {
        try {
            size_t len = tmp_buffer.pos() +
                         m_connection.receivePacket(remote_endpoint,
                                                    boost::asio::buffer(tmp_buffer.data(), tmp_buffer.capacity()));

            // Get source address.
            std::string address = remote_endpoint.address().to_string() + ":" + std::to_string(remote_endpoint.port());

            // Create a new manager.
            if (m_client_managers.find(address) == m_client_managers.end())
                m_client_managers[address] = new NetworkClientManager(this, remote_endpoint);

            // Get manager buffer.
            Buffer &buffer = m_client_managers[address]->buffer;

            // Append to manager buffer.
            buffer.appendBuffer(tmp_buffer, len);
            len = buffer.pos();
            buffer.setPos(0);

            // Decode packets.
            while (buffer.pos() < len) {
                size_t   packet_offset = buffer.pos();
                uint16_t packet_len    = buffer.readU16();

                // Ensure packet size.
                if (packet_len > buffer.capacity())
                    throw std::runtime_error("Too big packet.");

                // Ensure packet entirety.
                if ((packet_offset + packet_len) > len) {
                    buffer.setPos(packet_offset);
                    break;
                }

                // Read packet header.
                uint16_t seq_client = buffer.readU16();
                uint16_t seq_server = buffer.readU16();

                // TODO: do something with these ?
                (void)seq_client;
                (void)seq_server;

                uint8_t packet_id = buffer.readU8();
                Packet *packet    = CreateClientPacket(packet_id);

                // Read packet data.
                packet->readPacket(&buffer);

                // Ensure read correctness.
                if ((buffer.pos() - packet_offset) != packet_len)
                    throw std::runtime_error("Read too many bytes.");

                // Enqueue packet.
                m_client_managers[address]->enqueue(packet);
            }

            // Move remaining to front.
            buffer.moveToFront(len);

        } catch (std::exception &e) {
            std::cerr << "[ERR][SERVER]: " << e.what() << std::endl;
        }
    }

    std::cerr << "DISCONNECTED." << std::endl;
}
