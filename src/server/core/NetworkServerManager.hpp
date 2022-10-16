#pragma once

#include <iostream>
#include <cstdint>
#include <boost/asio.hpp>
#include "NetworkClientManager.hpp"
#include "../packets/connection_state.hpp"

class NetworkServerManager
{
public:
    NetworkServerManager(uint16_t port)
        : m_socket(m_io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {}

    void run()
    {
        Buffer buffer(4096);
        boost::asio::ip::udp::endpoint remote_endpoint;

        while (m_socket.is_open()) {
            try {
                size_t len = buffer.pos() + m_socket.receive_from(
                    boost::asio::buffer(buffer.data() + buffer.pos(), buffer.capacity() - buffer.pos()),
                    remote_endpoint
                );

                // Get source address.
                std::string address = remote_endpoint.address().to_string() + ":" + std::to_string(remote_endpoint.port());

                // Create a new manager.
                if (m_client_managers.find(address) == m_client_managers.end())
                    m_client_managers[address] = new NetworkClientManager(this, remote_endpoint);

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
                    Packet *packet      = CreateClientPacket(packet_id);

                    // Read packet data.
                    packet->readPacket(&buffer);

                    // Enqueue packet.
                    m_client_managers[address]->enqueue(packet);
                }

                // Move remaining to front.
                buffer.moveToFront(len);

            } catch (std::exception &e) {
                std::cerr << "ERR: " << e.what() << std::endl;
            }
        }

        std::cerr << "DISCONNECTED." << std::endl;
    }

    void broadcast(Packet *packet)
    {
        for (std::pair<std::string, NetworkClientManager*> entry : m_client_managers) {
            entry.second->send(packet);
        }
    }

    void processPackets()
    {
        for (std::pair<std::string, NetworkClientManager*> entry : m_client_managers) {
            entry.second->processPackets();
        }
    }

    void close()
    {
        m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_receive);
        m_socket.close();
    }

    boost::asio::ip::udp::socket &socket()
    {
        return m_socket;
    }

private:
    boost::asio::io_context                         m_io_context;
    boost::asio::ip::udp::socket                    m_socket;
    std::map<std::string, NetworkClientManager*>    m_client_managers;
};
