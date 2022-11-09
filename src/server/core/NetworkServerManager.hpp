#pragma once

#include <stdexcept>
#include <iostream>
#include <cstdint>
#include <boost/asio.hpp>
#include "NetworkClientManager.hpp"
#include "../packets/connection_state.hpp"
#include "../../reliable-udp/ReliableUdpConnection.hpp"

class NetworkServerManager
{
public:
    explicit NetworkServerManager(uint16_t port)
    {
        m_connection.prepareReceiving(port);
    }

    void run();

    void broadcast(Packet *packet)
    {
        for (std::pair<std::string, NetworkClientManager *> entry : m_client_managers) {
            entry.second->send(packet);
        }
    }

    void processPackets()
    {
        for (std::pair<std::string, NetworkClientManager *> entry : m_client_managers) {
            entry.second->processPackets();
        }
    }

    void close() {}

    ReliableUdpConnection &connection()
    {
        return m_connection;
    }

private:
    ReliableUdpConnection                         m_connection;
    std::map<std::string, NetworkClientManager *> m_client_managers;
};
