#include <memory>
#include "NetworkClientManager.hpp"
#include "NetworkServerManager.hpp"
#include "../../client/core/Coordinator.hpp"
#include "../../client/components/Player.hpp"
#include "../components/Network.hpp"
#include "../components/Transform.hpp"
#include "../components/Movement.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Weapon.hpp"
#include "../systems/ObjectsSystem.hpp"

extern Coordinator                    gCoordinator;
extern std::shared_ptr<ObjectsSystem> gObjectsSystem;

NetworkClientManager::NetworkClientManager(NetworkServerManager *server, boost::asio::ip::udp::endpoint endpoint)
    : buffer(4096), m_server(server), m_remote_endpoint(std::move(endpoint)), m_logged(true)
{
    // Broadcast entities to client.
    for (auto const &entity : gObjectsSystem->GetEntities()) {
        auto &transform = gCoordinator.GetComponent<Transform>(entity);

        // Ignore dead players.
        if (transform.type == EntityType::PLAYER && gCoordinator.GetComponent<Player>(entity).life == 0)
            continue;

        this->send(new PacketServerEntityCreate(transform.type, EntityTeam::PLAYER, transform.mobType, entity, transform.posX,
                                                transform.posY, 100));
    }

    m_entity = gCoordinator.CreateEntity();
    gCoordinator.AddComponent<Network>(m_entity, Network{this});
    gCoordinator.AddComponent<Player>(m_entity, Player{"Pepo", 100, 0});
    gCoordinator.AddComponent<Transform>(m_entity, Transform{EntityType::PLAYER, MobType::NONE, 50, 50});
    gCoordinator.AddComponent<Movement>(m_entity, Movement{0, 0, 0});
    gCoordinator.AddComponent<RigidBody>(m_entity, RigidBody{33, 17});
    gCoordinator.AddComponent<Weapon>(m_entity,
                                      Weapon{15, 0, Weapon::Type::MISSILETHROWER, Weapon::Team::PLAYERS, false});

    // Send entity ID to client.
    this->send(new PacketServerLogin(LoginState::ACCEPT, m_entity));

    // Broadcast entity creation.
    m_server->broadcast(new PacketServerEntityCreate(EntityType::PLAYER, EntityTeam::PLAYER, MobType::NONE, m_entity, 50, 50, 100));
}

void NetworkClientManager::send(Packet *packet)
{
    Buffer buffer(4096);
    size_t packetSize;

    // Encode packet to buffer.
    buffer.setPos(2);
    buffer.writeU16(0);
    buffer.writeU16(0);
    buffer.writeU8(GetServerPacketId(packet));
    packet->writePacket(&buffer);
    packetSize = buffer.pos();
    buffer.setPos(0);
    buffer.writeU16(static_cast<uint16_t>(packetSize));

    // Send packet.
    m_server->socket().send_to(boost::asio::buffer(buffer.data(), packetSize), m_remote_endpoint);
}

void NetworkClientManager::processClientLogin(PacketClientLogin *packet) {}

void NetworkClientManager::processClientKeepAlive(PacketClientKeepAlive *packet) {}

void NetworkClientManager::processClientInput(PacketClientInput *packet)
{
    if (m_logged) {
        gCoordinator.GetComponent<Network>(m_entity).inputs = packet->inputs;
    }
}

void NetworkClientManager::processClientPos(PacketClientPos *packet)
{
    if (m_logged) {
        auto &network   = gCoordinator.GetComponent<Network>(m_entity);
        auto &transform = gCoordinator.GetComponent<Transform>(m_entity);

        // Update client position. (Welcome cheaters !)
        transform.posX = packet->posX;
        transform.posY = packet->posY;

        // Reset timeout.
        network.timeout = 0;
    }
}

void NetworkClientManager::processClientLogout(PacketClientLogout *packet)
{
    (void)packet;
    if (m_logged) {
        // Set logged.
        m_logged = false;

        // Destroy client entity.
        gCoordinator.DestroyEntity(m_entity);

        // Broadcast entity destroy if alive.
        if (gCoordinator.GetComponent<Player>(m_entity).life > 0) {
            PacketServerEntityDestroy sentPacket(m_entity);

            m_server->broadcast(&sentPacket);
        }
    }
}

void NetworkClientManager::disconnect()
{
    if (m_logged) {
        m_queue_in.enqueue(new PacketClientLogout());
    }
}
