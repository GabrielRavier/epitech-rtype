#pragma once

#include <string>
#include <cstdint>
#include <boost/asio.hpp>

using namespace boost::asio;

class NetworkManager
{
public:
    NetworkManager() {}
    ~NetworkManager() = default;

    

private:
    ip::udp::endpoint   m_remote_endpoint;
    uint32_t            m_seq;
    uint16_t            m_entity_id;
    std::string         m_username;
    bool                m_logged;
};
