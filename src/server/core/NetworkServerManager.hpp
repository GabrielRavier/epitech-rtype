#pragma once

#include <stdexcept>
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

    void run();

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
