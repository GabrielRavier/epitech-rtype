#pragma once

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/crc.hpp>
#include <chrono>
#include <condition_variable>
#include <thread>
#include <map>
#include <unordered_map>
#include <deque>
#include <set>
#include <string_view>
#include <optional>

class ReliableUdpConnection
{
    // The io_context used by the manager
    boost::asio::io_context ioContext;

    struct IndividualReliableUdpConnection
    {
        struct PacketToSend
        {
            std::vector<unsigned char>                           data;
            std::optional<std::chrono::steady_clock::time_point> lastSendTime;
            std::size_t                                          sendAttempts = 0;
        };

        std::deque<unsigned char>                           bufferedBytes;
        std::uint64_t                                       remoteNextExpectedPacketId = 0;
        std::uint64_t                                       nextSentPacketId           = 1;
        std::map<std::uint64_t, std::vector<unsigned char>> availablePackets;
        std::map<std::uint64_t, PacketToSend>               packetsToSend;
        std::uint64_t                                       nextUnreadPacketId = 1;
        std::set<std::uint64_t>                             remoteAcknowledgedPacketsAboveNextExpectedPacketId;
        std::optional<boost::uuids::uuid>                   uuid;
        bool                                                isConnectionDead = false;
        std::chrono::time_point<std::chrono::steady_clock>  lastPacketTime   = std::chrono::steady_clock::now();

        std::uint64_t getAckNextExpectedPacketId()
        {
            std::uint64_t result = std::max(this->nextUnreadPacketId, static_cast<std::uint64_t>(1)) - 1;
            for (auto &i : this->availablePackets)
                if (i.first == result + 1)
                    result = i.first;
                else
                    break;
            ++result;
            return result;
        }

        void handleReceivedAckNextExpectedPacketId(std::uint64_t receivedAckNextExpectedPacketId)
        {
            if (receivedAckNextExpectedPacketId > this->remoteNextExpectedPacketId) {
                this->remoteNextExpectedPacketId = receivedAckNextExpectedPacketId;
                this->remoteAcknowledgedPacketsAboveNextExpectedPacketId.erase(
                    this->remoteAcknowledgedPacketsAboveNextExpectedPacketId.begin(),
                    this->remoteAcknowledgedPacketsAboveNextExpectedPacketId.lower_bound(
                        receivedAckNextExpectedPacketId));
            }
        }

        void killThisConnection()
        {
            this->isConnectionDead = true;
            this->bufferedBytes.clear();
            this->availablePackets.clear();
            this->remoteAcknowledgedPacketsAboveNextExpectedPacketId.clear();
        }

        void checkLastPacket()
        {
            if (this->lastPacketTime + std::chrono::seconds(5) < std::chrono::steady_clock::now())
                this->killThisConnection();
        }
    };

    static constexpr std::string_view packetBeginMagic = "\x7FRUP";
    static constexpr std::size_t packetHeaderSize = ReliableUdpConnection::packetBeginMagic.size() + // Packet magic
                                                    sizeof(std::uint64_t) +                          // Packet checksum
                                                    boost::uuids::uuid::static_size() + // Packet connection UUID
                                                    sizeof(std::uint8_t) +              // Packet flags
                                                    sizeof(std::uint64_t) +             // Packet ID
                                                    sizeof(std::uint64_t) + // Packet ACK next expected packet ID
                                                    sizeof(std::uint64_t);  // Packet data size

    // This mutex protects individualConnections and must ALWAYS be locked when using individualConnections
    std::mutex individualConnectionsMutex;

    // This condition_variable is notified by the receiver thread whenever it has received something new for any thread
    // to look at Note that this must always be waited on using the individualConnectionsMutex mutex
    std::condition_variable individualConnectionsCondVar;

    // This contains all the data relating to individual connections. individualConnectionsMutex must ALWAYS be locked
    // before using this
    std::unordered_map<boost::asio::ip::udp::endpoint, IndividualReliableUdpConnection> individualConnections;

    // The socket used to send and receive data
    boost::asio::ip::udp::socket socket;

    // If this is set to true, then all threads will stop as soon as they can
    std::atomic<bool> allThreadsShouldStop = false;

    // Contains the thread that manages receiving data when we have a connection up
    std::optional<std::thread> receiveThread;

    // Ensures the receiver thread is only started once
    std::once_flag beginReceiveThreadOnceFlag;

    // Contains the thread that manages sending data when we have a connection up
    std::optional<std::thread> sendThread;

    // Ensures the sender thread is only started once
    std::once_flag beginSendThreadOnceFlag;

    bool isReceiver      = false;
    using our_crc64_type = boost::crc_optimal<64, 0x42F0E1EBA9EA3693>;

    // This is the raw function that sends and receives packets
    // data is a buffer containing the raw data of the packet to be sent
    // connectionUuid is the uuid of the connection corresponding to the send packet
    // isInitialPacket must only be true if this function is being used to send the initial packet of a connection
    bool rawSendPacket(boost::asio::const_buffer data, const boost::asio::ip::udp::endpoint &target_endpoint,
                       const boost::uuids::uuid &connectionUuid, bool isInitialPacket, bool isAcknowledgmentPacket,
                       bool isLastPacket, std::uint64_t ackNextExpectedPacketId, std::uint64_t packetId);

    void receiveThreadFunction();
    void beginReceiveThread();

    void sendThreadFunction();
    void beginSendThread();

public:
    ReliableUdpConnection() : socket(ioContext, boost::asio::ip::udp::v4()) {}

    ~ReliableUdpConnection();

    // This opens a new connection to the given host and port
    // Typically, this is used by clients to connect to a server
    boost::asio::ip::udp::endpoint openConnection(std::string_view host, std::uint16_t port);

    // This makes it so this ReliableUdpConnection is assigned to the given port
    // Typically, this is used by servers who want to receive data from arbitrary peers (i.e. manage an "infinite"
    // amount of connections)
    void prepareReceiving(std::uint16_t port);

    void sendPacket(const boost::asio::ip::udp::endpoint &endpoint, boost::asio::const_buffer data);

    std::size_t receivePacket(boost::asio::ip::udp::endpoint &endpoint, boost::asio::mutable_buffer result,
                              bool isBlocking = true);

    bool isOpen();

    void close();
};
