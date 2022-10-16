#pragma once

#include <cstdint>
#include <boost/asio.hpp>
#include "../packets/network_handler.hpp"
#include "../sync_queue.hpp"

class NetworkServerManager;

class NetworkClientManager : private INetworkHandler
{
public:
    NetworkClientManager(NetworkServerManager *server, boost::asio::ip::udp::endpoint endpoint)
        : m_server(server), m_remote_endpoint(endpoint)
    {}

    ~NetworkClientManager() = default;

    void enqueue(Packet* packet)
    {
        m_queue_in.enqueue(packet);
    }

    void send(Packet *packet);

    void processPackets()
    {
        while (m_queue_in.count()) {
            Packet *packet = m_queue_in.dequeue();
            packet->processPacket(this);
            delete packet;
        }
    }

private:
    void processClientLogin(PacketClientLogin *packet);
    void processClientKeepAlive(PacketClientKeepAlive *packet);
    void processClientInput(PacketClientInput *packet);

    void processServerLogin(PacketServerLogin *packet) {}
    void processServerKeepAlive(PacketServerKeepAlive *packet) {}
    void processServerEntityCreate(PacketServerEntityCreate*packet) {}
    void processServerUpdateHealth(PacketServerUpdateHealth *packet) {}
    void processServerUpdatePos(PacketServerUpdatePos *packet) {}
    void processServerUpdateScore(PacketServerUpdateScore *packet) {}

private:
    NetworkServerManager            *m_server;
    SynchronisedQueue<Packet*>      m_queue_in;
    boost::asio::ip::udp::endpoint  m_remote_endpoint;
    uint16_t                        m_entity_id;
};
