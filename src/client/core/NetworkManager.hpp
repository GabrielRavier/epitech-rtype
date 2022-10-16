#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "../../server/sync_queue.hpp"
#include "../../server/packets/connection_state.hpp"

class NetworkManager
{
public:
    NetworkManager(const char *host, uint16_t port)
        : m_socket(m_io_context), m_seq_client(0), m_seq_server(0)
    {
        boost::asio::ip::udp::resolver resolver(m_io_context);

        // Set target endpoint and port.
        m_target_endpoint = *resolver.resolve(boost::asio::ip::udp::v4(), host, "").begin();
        m_target_endpoint.port(port);

        // Open socket.
        m_socket.open(boost::asio::ip::udp::v4());
    }

    void send(Packet *packet)
    {
        Buffer *buffer = new Buffer(1024);
        size_t packetSize;

        // Encode packet to buffer.
        buffer->setPos(2);
        buffer->writeU16(m_seq_client++);
        buffer->writeU16(m_seq_server);
        buffer->writeU8(GetClientPacketId(packet));
        packet->writePacket(buffer);
        packetSize = buffer->pos();
        buffer->setPos(0);
        buffer->writeU16(static_cast<uint16_t>(packetSize - 2));
        buffer->setPos(packetSize);

        // Send packet.
        m_socket.send_to(boost::asio::buffer(buffer->data(), packetSize), m_target_endpoint);

        // Queue packet for acknowledgment.
        m_queue_out.enqueue(buffer);
    }

    void run()
    {
        Buffer buffer(4096);

        try {
            while (m_socket.is_open()) {
                size_t len = buffer.pos() + m_socket.receive_from(
                    boost::asio::buffer(buffer.data() + buffer.pos(), buffer.capacity() - buffer.pos()),
                    m_target_endpoint
                );

                // Set buffer to begin.
                buffer.setPos(0);

                // Decode packets.
                while (buffer.pos() < len) {
                    uint16_t packet_len = buffer.readU16();

                    // Ensure packet size.
                    if ((packet_len + 2) > buffer.capacity())
                        throw std::exception("Too big packet.");

                    // Ensure packet entirety.
                    if ((buffer.pos() + packet_len) > len) {
                        buffer.setPos(buffer.pos() - 2);
                        break;
                    }

                    // Read packet header.
                    uint16_t seq_client = buffer.readU16();
                    uint16_t seq_server = buffer.readU16();
                    uint8_t packet_id   = buffer.readU8();
                    Packet *packet      = CreateServerPacket(packet_id);

                    // Read packet data.
                    packet->readPacket(&buffer);

                    // Enqueue packet.
                    m_queue_in.enqueue(packet);
                }

                // Move remaining to front.
                buffer.moveToFront(len);
            }

        } catch (std::exception &e) {
            std::cerr << "ERR: " << e.what() << std::endl;
        }

        std::cerr << "DISCONNECTED." << std::endl;
    }

    void close()
    {
        m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_receive);
        m_socket.close();
    }

private:
    boost::asio::io_context         m_io_context;
    boost::asio::ip::udp::endpoint  m_target_endpoint;
    boost::asio::ip::udp::socket    m_socket;
    uint16_t                        m_seq_client;
    uint16_t                        m_seq_server;
    SynchronisedQueue<Packet*>      m_queue_in;
    SynchronisedQueue<Buffer*>      m_queue_out;
};
