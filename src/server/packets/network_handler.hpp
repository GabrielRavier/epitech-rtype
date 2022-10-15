#pragma once

#include "packet.hpp"
#include "packet_client_login.hpp"
#include "packet_client_keep_alive.hpp"
#include "packet_client_input.hpp"
#include "packet_server_login.hpp"
#include "packet_server_keep_alive.hpp"
#include "packet_server_entity_create.hpp"
#include "packet_server_update_health.hpp"
#include "packet_server_update_pos.hpp"
#include "packet_server_update_score.hpp"

class INetworkHandler
{
public:
    INetworkHandler() {};
    virtual ~INetworkHandler()                                                  = default;

    virtual void processClientLogin(PacketClientLogin *packet)                  = 0;
    virtual void processClientKeepAlive(PacketClientKeepAlive *packet)          = 0;
    virtual void processClientInput(PacketClientInput *packet)                  = 0;

    virtual void processServerLogin(PacketServerLogin *packet)                  = 0;
    virtual void processServerKeepAlive(PacketServerKeepAlive *packet)          = 0;
    virtual void processServerEntityCreate(PacketServerEntityCreate*packet)     = 0;
    virtual void processServerUpdateHealth(PacketServerUpdateHealth *packet)    = 0;
    virtual void processServerUpdatePos(PacketServerUpdatePos *packet)          = 0;
    virtual void processServerUpdateScore(PacketServerUpdateScore *packet)      = 0;
};
