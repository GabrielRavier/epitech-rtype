#pragma once

#include <cstdint>
#include <boost/asio.hpp>
#include "INetworkHandler.hpp"
#include "../sync_queue.hpp"
#include "../../client/core/Types.hpp"

class NetworkServerManager;

class NetworkClientManager : private INetworkHandler
{
public:
    Buffer buffer;

public:
    NetworkClientManager(NetworkServerManager *server, boost::asio::ip::udp::endpoint endpoint);
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
    void processClientPos(PacketClientPos *packet);
    void processClientLogout(PacketClientLogout *packet);

    void processServerLogin(PacketServerLogin *packet) {}
    void processServerKeepAlive(PacketServerKeepAlive *packet) {}
    void processServerEntityCreate(PacketServerEntityCreate *packet) {}
    void processServerEntityDestroy(PacketServerEntityDestroy *packet) {}
    void processServerUpdateHealth(PacketServerUpdateHealth *packet) {}
    void processServerUpdatePos(PacketServerUpdatePos *packet) {}
    void processServerUpdateScore(PacketServerUpdateScore *packet) {}

private:
    NetworkServerManager            *m_server;
    boost::asio::ip::udp::endpoint  m_remote_endpoint;
    SynchronisedQueue<Packet*>      m_queue_in;
    Entity                          m_entity;
    bool                            m_logged;
};
