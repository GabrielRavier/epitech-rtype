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
    boost::asio::io_service         m_io_service;
    boost::asio::ip::udp::endpoint  m_target_endpoint;
    boost::asio::ip::udp::socket    m_socket;

public:
    NetworkManager(const char *host, size_t port) : m_socket(m_io_service)
    {
        boost::asio::ip::udp::resolver resolver(m_io_service);
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host, "");

        // Set target endpoint and port.
        m_target_endpoint = *resolver.resolve(query);
        m_target_endpoint.port(port);

        // Open socket.
        m_socket.open(boost::asio::ip::udp::v4());
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

        boost::array<char, 1> send_buf  = { 0 };
        m_socket.send_to(boost::asio::buffer(send_buf), m_target_endpoint);

        while (m_socket.is_open()) {
        std::cout << "test" << std::endl;
            m_socket.receive_from(buffer, sender_endpoint);
            std::cout <<  "ok" << std::endl;
        }
    }

    void close()
    {
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_receive);
        m_socket.close();
    }
};
