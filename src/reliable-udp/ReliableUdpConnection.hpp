#pragma once

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/endian.hpp>
#include <boost/crc.hpp>
#include <chrono>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <map>
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
        std::deque<unsigned char>                           bufferedBytes;
        std::uint64_t                                       remoteNextExpectedPacketId = 0;
        std::uint64_t                                       nextSentPacketId           = 1;
        std::map<std::uint64_t, std::vector<unsigned char>> availablePackets;
        std::uint64_t                                       nextExpectedPacketId = 1;
        std::uint64_t                                       nextUnreadPacketId   = 1;
        std::set<std::uint64_t>                             remoteAcknowledgedPacketsAboveNextExpectedPacketId;
        std::optional<boost::uuids::uuid>                   uuid;
        bool                                                isConnectionDead = false;
        std::chrono::time_point<std::chrono::steady_clock>  lastPacketTime   = std::chrono::steady_clock::now();

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

    // The condition variable (which is to be used with the preceding mutex) will be repeatedly notified upon closure
    // until all threads stop
    std::mutex              allThreadsShouldStopNotificationMutex;
    std::condition_variable allThreadShouldStopNotification;

    // This counts the amount of packet threads that are currently being run. The destructor waits for this to be 0
    // before exiting
    std::atomic<std::uintmax_t> packetThreadsInFlight = 0;

    // Contains the thread that manages receiving data when we have a connection up
    std::optional<std::thread> receiveThread;

    // Ensures the receiver thread is only started once
    std::once_flag beginReceiveThreadOnceFlag;

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

    void sendPacket(const boost::asio::ip::udp::endpoint &endpoint, boost::asio::const_buffer data,
                    bool isBlocking = false);

    std::size_t receivePacket(boost::asio::ip::udp::endpoint &endpoint, boost::asio::mutable_buffer result,
                              bool isBlocking = true);

    bool isOpen();

    void close();
};
