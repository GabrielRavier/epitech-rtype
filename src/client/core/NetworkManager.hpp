#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "../../server/sync_queue.hpp"
#include "../../server/packets/packet.hpp"

class NetworkManager
{
private:
    SynchronisedQueue<Packet>       m_queue;
    boost::asio::io_context         m_io_context;
    boost::asio::ip::udp::endpoint  m_target_endpoint;
    boost::asio::ip::udp::socket    m_socket;

public:
    NetworkManager(const char *host, uint16_t port) : m_socket(m_io_context)
    {
        boost::asio::ip::udp::resolver resolver(m_io_context);

        // Set target endpoint and port.
        m_target_endpoint = *resolver.resolve(boost::asio::ip::udp::v4(), host, "").begin();
        m_target_endpoint.port(port);

        // Bind socket to endpoint.
        m_socket.connect(m_target_endpoint);
    }

    void send(char *buf, size_t len)
    {
        m_socket.send_to(boost::asio::buffer(buf, len), m_target_endpoint);
    }

    void run()
    {
        uint8_t data[4096];
        boost::asio::mutable_buffer buffer = boost::asio::buffer(data, 4096);
        boost::asio::ip::udp::endpoint sender_endpoint;

        try {
            while (m_socket.is_open()) {
                m_socket.receive_from(buffer, sender_endpoint);
            }

        } catch (std::exception &e) {
            std::cerr << "ERR: " << e.what() << std::endl;
        }
    }

    void close()
    {
        m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_receive);
        m_socket.close();
    }
};
