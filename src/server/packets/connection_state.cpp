#include <stdexcept>
#include <typeindex>
#include "connection_state.hpp"
#include "packet_client_login.hpp"
#include "packet_client_logout.hpp"
#include "packet_client_keep_alive.hpp"
#include "packet_client_input.hpp"
#include "packet_client_pos.hpp"
#include "packet_server_login.hpp"
#include "packet_server_keep_alive.hpp"
#include "packet_server_entity_create.hpp"
#include "packet_server_entity_destroy.hpp"
#include "packet_server_update_health.hpp"
#include "packet_server_update_pos.hpp"
#include "packet_server_update_score.hpp"

template <std::size_t N, class T>
constexpr std::size_t countof(T (&/*unused*/)[N])
{
    return N;
}

template <typename T, typename = std::enable_if<std::is_base_of<Packet, T>::value>>
Packet *PacketInstantiator()
{
    return new T();
};

using instantiator_ptr = Packet *(*)();

static inline instantiator_ptr gClientPacketsToPacket[] = {
    &PacketInstantiator<PacketClientLogin>,  &PacketInstantiator<PacketClientKeepAlive>,
    &PacketInstantiator<PacketClientInput>,  &PacketInstantiator<PacketClientPos>,
    &PacketInstantiator<PacketClientLogout>,
};

static inline instantiator_ptr gServerPacketsToPacket[] = {
    &PacketInstantiator<PacketServerLogin>,        &PacketInstantiator<PacketServerKeepAlive>,
    &PacketInstantiator<PacketServerEntityCreate>, &PacketInstantiator<PacketServerEntityDestroy>,
    &PacketInstantiator<PacketServerUpdateHealth>, &PacketInstantiator<PacketServerUpdatePos>,
    &PacketInstantiator<PacketServerUpdateScore>,
};

static inline std::type_index gClientPacketsToId[] = {
    typeid(PacketClientLogin), typeid(PacketClientKeepAlive), typeid(PacketClientInput),
    typeid(PacketClientPos),   typeid(PacketClientLogout),
};

static inline std::type_index gServerPacketsToId[] = {
    typeid(PacketServerLogin),         typeid(PacketServerKeepAlive),    typeid(PacketServerEntityCreate),
    typeid(PacketServerEntityDestroy), typeid(PacketServerUpdateHealth), typeid(PacketServerUpdatePos),
    typeid(PacketServerUpdateScore),
};

uint8_t GetClientPacketId(Packet *packet)
{
    std::type_index type = typeid(*packet);

    for (uint8_t i = 0; i < countof(gClientPacketsToId); i++)
        if (gClientPacketsToId[i] == type)
            return (i);

    throw std::runtime_error("Invalid packet type.");
}

uint8_t GetServerPacketId(Packet *packet)
{
    std::type_index type = typeid(*packet);

    for (uint8_t i = 0; i < countof(gServerPacketsToId); i++)
        if (gServerPacketsToId[i] == type)
            return (i);

    throw std::runtime_error("Invalid packet type.");
}

Packet *CreateClientPacket(uint8_t id)
{
    if (id >= countof(gClientPacketsToPacket))
        throw std::runtime_error("Invalid packet ID.");

    return gClientPacketsToPacket[id]();
}

Packet *CreateServerPacket(uint8_t id)
{
    if (id >= countof(gServerPacketsToPacket))
        throw std::runtime_error("Invalid packet ID.");

    return gServerPacketsToPacket[id]();
}
