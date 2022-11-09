#pragma once

#include <iostream>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "../../server/sync_queue.hpp"
#include "../../server/packets/connection_state.hpp"
#include "../../server/core/INetworkHandler.hpp"
#include "../../reliable-udp/ReliableUdpConnection.hpp"

class NetworkManager : private INetworkHandler
{
public:
    NetworkManager() = default;

    NetworkManager(const char *host, uint16_t port)
    {
        this->Init(host, port);
    }

    void Init(const char *host, uint16_t port)
    {
        m_target_endpoint = m_connection.openConnection(host, port);
    }

    void run()
    {
        Buffer buffer(4096);

        try {
            while (m_connection.isOpen()) {
                const size_t len =
                    buffer.pos() + m_connection.receivePacket(m_target_endpoint,
                                                              boost::asio::buffer(buffer.data() + buffer.pos(),
                                                                                  buffer.capacity() - buffer.pos()));

                // Set buffer to begin.
                buffer.setPos(0);

                // Decode packets.
                while (buffer.pos() < len) {
                    const size_t   packet_offset = buffer.pos();
                    const uint16_t packet_len    = buffer.readU16();

                    // Ensure packet size.
                    if (packet_len > buffer.capacity())
                        throw std::runtime_error("Too big packet.");

                    // Ensure packet entirety.
                    if ((packet_offset + packet_len) > len) {
                        buffer.setPos(packet_offset);
                        break;
                    }

                    // Read packet header.
                    const uint16_t seq_client = buffer.readU16();
                    const uint16_t seq_server = buffer.readU16();

                    // TODO: Do something with this ?
                    (void)seq_client;
                    (void)seq_server;

                    const uint8_t packet_id = buffer.readU8();
                    auto         *packet    = CreateServerPacket(packet_id);

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

    void send(Packet *packet, bool isBlocking = false)
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
        m_connection.sendPacket(m_target_endpoint, boost::asio::buffer(buffer.data(), packetSize), isBlocking);
    }

    void processPackets()
    {
        while (m_queue_in.count() != 0) {
            Packet *packet = m_queue_in.dequeue();
            packet->processPacket(this);
            delete packet;
        }
    }

    void close()
    {
        auto packet = PacketClientLogout();
        for (int i = 0; i < 10; ++i)
            this->send(&packet);
        m_connection.close();
    }

private:
    void processClientLogin(PacketClientLogin *packet) override {}

    void processClientKeepAlive(PacketClientKeepAlive *packet) override {}

    void processClientInput(PacketClientInput *packet) override {}

    void processClientPos(PacketClientPos *packet) override {}

    void processClientLogout(PacketClientLogout *packet) override {}

    void processServerLogin(PacketServerLogin *packet) override;
    void processServerKeepAlive(PacketServerKeepAlive *packet) override;
    void processServerEntityCreate(PacketServerEntityCreate *packet) override;
    void processServerEntityDestroy(PacketServerEntityDestroy *packet) override;
    void processServerUpdateHealth(PacketServerUpdateHealth *packet) override;
    void processServerUpdatePos(PacketServerUpdatePos *packet) override;
    void processServerUpdateScore(PacketServerUpdateScore *packet) override;

private:
    ReliableUdpConnection          m_connection;
    boost::asio::ip::udp::endpoint m_target_endpoint;
    SynchronisedQueue<Packet *>    m_queue_in;
    SynchronisedQueue<Buffer *>    m_queue_out;
};
