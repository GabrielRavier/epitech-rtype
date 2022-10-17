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
    ~NetworkClientManager() override = default;

    void enqueue(Packet *packet)
    {
        m_queue_in.enqueue(packet);
    }

    void send(Packet *packet);

    void processPackets()
    {
        while (m_queue_in.count() != 0) {
            Packet *packet = m_queue_in.dequeue();
            packet->processPacket(this);
            delete packet;
        }
    }

private:
    void processClientLogin(PacketClientLogin *packet) override;
    void processClientKeepAlive(PacketClientKeepAlive *packet) override;
    void processClientInput(PacketClientInput *packet) override;
    void processClientPos(PacketClientPos *packet) override;
    void processClientLogout(PacketClientLogout *packet) override;

    void processServerLogin(PacketServerLogin *packet) override {}

    void processServerKeepAlive(PacketServerKeepAlive *packet) override {}

    void processServerEntityCreate(PacketServerEntityCreate *packet) override {}

    void processServerEntityDestroy(PacketServerEntityDestroy *packet) override {}

    void processServerUpdateHealth(PacketServerUpdateHealth *packet) override {}

    void processServerUpdatePos(PacketServerUpdatePos *packet) override {}

    void processServerUpdateScore(PacketServerUpdateScore *packet) override {}

private:
    NetworkServerManager          *m_server;
    boost::asio::ip::udp::endpoint m_remote_endpoint;
    SynchronisedQueue<Packet *>    m_queue_in;
    Entity                         m_entity;
    bool                           m_logged;
};
