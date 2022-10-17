#pragma once

#include <stdexcept>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "../../server/sync_queue.hpp"
#include "../../server/packets/connection_state.hpp"
#include "../../server/core/INetworkHandler.hpp"

class NetworkManager : private INetworkHandler
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
        m_socket.connect(m_target_endpoint);
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
                    size_t packet_offset    = buffer.pos();
                    uint16_t packet_len     = buffer.readU16();

                    // Ensure packet size.
                    if (packet_len > buffer.capacity())
                        throw std::runtime_error("Too big packet.");

                    // Ensure packet entirety.
                    if ((buffer.pos() + packet_len) > len) {
                        buffer.setPos(packet_offset);
                        break;
                    }

                    // Read packet header.
                    uint16_t seq_client = buffer.readU16();
                    uint16_t seq_server = buffer.readU16();
                    uint8_t packet_id   = buffer.readU8();
                    Packet *packet      = CreateServerPacket(packet_id);

                    // Read packet data.
                    packet->readPacket(&buffer);

                    // Ensure read correctness.
                    if ((buffer.pos() - packet_offset) != packet_len)
                        throw std::runtime_error("Read too many bytes.");

                    // Enqueue packet.
                    m_queue_in.enqueue(packet);
                }

                // Move remaining to front.
                buffer.moveToFront(len);
            }

        } catch (std::exception &e) {
            std::cerr << "[ERR][CLIENT]: " << e.what() << std::endl;
        }

        std::cerr << "DISCONNECTED." << std::endl;
    }

    void send(Packet *packet)
    {
        Buffer buffer(4096);
        size_t packetSize;

        // Encode packet to buffer.
        buffer.setPos(2);
        buffer.writeU16(0);
        buffer.writeU16(0);
        buffer.writeU8(GetClientPacketId(packet));
        packet->writePacket(&buffer);
        packetSize = buffer.pos();
        buffer.setPos(0);
        buffer.writeU16(static_cast<uint16_t>(packetSize));

        // Send packet.
        m_socket.send_to(boost::asio::buffer(buffer.data(), packetSize), m_target_endpoint);
    }

    void processPackets()
    {
        while (m_queue_in.count()) {
            Packet *packet = m_queue_in.dequeue();
            packet->processPacket(this);
            delete packet;
        }
    }

    void close()
    {
        m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_receive);
        m_socket.close();
    }

private:
    void processClientLogin(PacketClientLogin *packet) {}
    void processClientKeepAlive(PacketClientKeepAlive *packet) {}
    void processClientInput(PacketClientInput *packet) {}
    void processClientPos(PacketClientPos *packet) {}
    void processClientLogout(PacketClientLogout *packet) {}

    void processServerLogin(PacketServerLogin *packet);
    void processServerKeepAlive(PacketServerKeepAlive *packet);
    void processServerEntityCreate(PacketServerEntityCreate *packet);
    void processServerEntityDestroy(PacketServerEntityDestroy *packet);
    void processServerUpdateHealth(PacketServerUpdateHealth *packet);
    void processServerUpdatePos(PacketServerUpdatePos *packet);
    void processServerUpdateScore(PacketServerUpdateScore *packet);

private:
    boost::asio::io_context         m_io_context;
    boost::asio::ip::udp::endpoint  m_target_endpoint;
    boost::asio::ip::udp::socket    m_socket;
    uint16_t                        m_seq_client;
    uint16_t                        m_seq_server;
    SynchronisedQueue<Packet*>      m_queue_in;
    SynchronisedQueue<Buffer*>      m_queue_out;
};
