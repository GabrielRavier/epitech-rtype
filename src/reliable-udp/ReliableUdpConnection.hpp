#pragma once

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/endian.hpp>
#include <boost/crc.hpp>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <map>
#include <deque>
#include <set>
#include <string_view>
#include <optional>

static constexpr bool gReliableUdpConnectionDebugPrints = false;

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
        bool                                                isConnectionDead;
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
    void rawSendPacket(boost::asio::const_buffer data, const boost::asio::ip::udp::endpoint &target_endpoint,
                       const boost::uuids::uuid &connectionUuid, bool isInitialPacket, bool isAcknowledgmentPacket,
                       bool isLastPacket, std::uint64_t ackNextExpectedPacketId, std::uint64_t packetId)
    {
        std::uint8_t flags = static_cast<std::uint8_t>(isInitialPacket) |
                             (static_cast<std::uint8_t>(isAcknowledgmentPacket) << 1) |
                             (static_cast<std::uint8_t>(isLastPacket) << 2);
        std::uint64_t encodedDataSize = boost::endian::native_to_little(static_cast<std::uint64_t>(data.size()));
        std::uint64_t encodedPacketId = boost::endian::native_to_little(packetId);
        std::uint64_t encodedAckNextExpectedPacketId = boost::endian::native_to_little(ackNextExpectedPacketId);

        std::uint64_t packetChecksum;

        std::array<boost::asio::const_buffer, 8> buffersToSend = {
            {{ReliableUdpConnection::packetBeginMagic.data(), ReliableUdpConnection::packetBeginMagic.size()},
             {&packetChecksum, sizeof(packetChecksum)},
             {connectionUuid.data, connectionUuid.size()},
             {&flags, sizeof(flags)},
             {&encodedPacketId, sizeof(encodedPacketId)},
             {&encodedAckNextExpectedPacketId, sizeof(encodedAckNextExpectedPacketId)},
             {&encodedDataSize, sizeof(encodedDataSize)},
             data}};
        ReliableUdpConnection::our_crc64_type crc64_checksum;

        // This starts at 2 because the checksum obviously can't include itself and doesn't have to check the magic
        // (since the check would never fire if it was wrong)
        for (std::size_t i = 2; i < buffersToSend.size(); ++i)
            crc64_checksum.process_bytes(buffersToSend[i].data(), buffersToSend[i].size());
        packetChecksum = boost::endian::native_to_little(static_cast<std::uint64_t>(crc64_checksum.checksum()));
        this->socket.send_to(buffersToSend, target_endpoint);
        if (gReliableUdpConnectionDebugPrints)
            std::cerr << std::this_thread::get_id() << " this->socket.send_to executed successfully\n";
    }

    void receiveThreadFunction()
    {
        if (gReliableUdpConnectionDebugPrints)
            std::cerr << "Starting receive thread\n";
        std::vector<unsigned char> receiveBuffer;
        receiveBuffer.resize(50000);

        std::thread ioContextThread([this] {
            while (!this->allThreadsShouldStop) {
                this->ioContext.restart();
                this->ioContext.run_for(std::chrono::milliseconds(50));
            }
        });

        std::mutex               mutForSem;
        std::condition_variable  sem;
        std::future<std::size_t> future;
        while (true) {
            boost::asio::ip::udp::endpoint endpoint;

            if (!future.valid())
                future = this->socket.async_receive_from(
                    boost::asio::mutable_buffer(receiveBuffer.data(), receiveBuffer.size()), endpoint,
                    boost::asio::use_future);
            auto status = future.wait_for(std::chrono::milliseconds(20));

            if (this->allThreadsShouldStop) {
                ioContextThread.join();
                return;
            }
            if (status == std::future_status::ready) {
                if (gReliableUdpConnectionDebugPrints)
                    std::cerr << std::this_thread::get_id()
                              << " this->socket.async_receive_from returned something !\n";
                auto futureResult = future.get();
                if (futureResult > receiveBuffer.size())
                    throw std::logic_error("wtf ???? receive_from should NEVER return higher than size just wtf");

                // If the endpoint we were given is 0, there's probably a problem...
                // Also if we've read 0 bytes there's no reason to bother doing anything
                if (endpoint.port() == 0 || futureResult == 0) {
                    if (gReliableUdpConnectionDebugPrints)
                        std::cerr << std::this_thread::get_id()
                                  << " Receiver: Got endpoint with port 0 or futureResult == 0\n";
                    continue;
                }

                {
                    std::lock_guard guardIndividualConnections(this->individualConnectionsMutex);

                    auto individualConnection = this->individualConnections.find(endpoint);
                    if (individualConnection == this->individualConnections.end()) {
                        if (!this->isReceiver)
                            continue;
                        individualConnection =
                            this->individualConnections
                                .insert(decltype(this->individualConnections)::value_type(endpoint, {}))
                                .first;
                    }
                    if (individualConnection->second.isConnectionDead)
                        continue;
                    individualConnection->second.bufferedBytes.insert(individualConnection->second.bufferedBytes.end(),
                                                                      receiveBuffer.begin(),
                                                                      receiveBuffer.begin() + futureResult);

                    while (true) {
                        // Check if we now have a packet magic within the data
                        auto magicLocation = std::search(individualConnection->second.bufferedBytes.begin(),
                                                         individualConnection->second.bufferedBytes.end(),
                                                         ReliableUdpConnection::packetBeginMagic.begin(),
                                                         ReliableUdpConnection::packetBeginMagic.end());

                        // If we don't, try to erase as much data as we can (up to 4 bytes before the end of the
                        // buffered data as we may have the beginning of a magic there otherwise)
                        if (individualConnection->second.bufferedBytes.end() == magicLocation) {
                            if (individualConnection->second.bufferedBytes.size() >
                                (ReliableUdpConnection::packetBeginMagic.size() + 4))
                                individualConnection->second.bufferedBytes.erase(
                                    individualConnection->second.bufferedBytes.begin(),
                                    individualConnection->second.bufferedBytes.end() -
                                        (ReliableUdpConnection::packetBeginMagic.size() + 2));
                            if (gReliableUdpConnectionDebugPrints)
                                std::cerr << std::this_thread::get_id() << " found no packet magic in the data\n";
                            break;
                        }

                        // Erase everything up to the packet magic
                        individualConnection->second.bufferedBytes.erase(
                            individualConnection->second.bufferedBytes.begin(), magicLocation);
                        if (individualConnection->second.bufferedBytes.at(0) != 0x7F)
                            throw std::logic_error(
                                "We found a packet magic and still individualConnection->second.bufferedBytes.at(0) != "
                                "0x7F is false ??");

                        // Is the header buffered ?
                        if (individualConnection->second.bufferedBytes.size() <
                            ReliableUdpConnection::packetHeaderSize) {
                            if (gReliableUdpConnectionDebugPrints)
                                std::cerr << std::this_thread::get_id() << " header not buffered yet\n";
                            break;
                        }

                        // If the packet size is above 5 MB, something has gone terribly wrong (likely corruption) as we
                        // don't accept stuff that big. Just ignore this packet
                        if (boost::endian::load_little_u64(&individualConnection->second.bufferedBytes.at(
                                ReliableUdpConnection::packetHeaderSize - 8)) > 5000000) {
                            individualConnection->second.bufferedBytes.erase(
                                individualConnection->second.bufferedBytes.begin());
                            if (gReliableUdpConnectionDebugPrints)
                                std::cerr << std::this_thread::get_id() << " packet was too large\n";
                            continue;
                        }

                        // Is the whole packet buffered ?
                        if (individualConnection->second.bufferedBytes.size() <
                            ReliableUdpConnection::packetHeaderSize +
                                boost::endian::load_little_u64(&individualConnection->second.bufferedBytes.at(
                                    ReliableUdpConnection::packetHeaderSize - 8))) {
                            if (gReliableUdpConnectionDebugPrints)
                                std::cerr << std::this_thread::get_id() << " whole packet not buffered yet\n";
                            break;
                        }

                        std::uint64_t                              receivedPacketChecksum;
                        boost::uuids::uuid                         receivedConnectionUuid;
                        std::uint8_t                               receivedFlags;
                        std::uint64_t                              receivedPacketId;
                        std::uint64_t                              receivedAckNextExpectedPacketId;
                        std::uint64_t                              receivedDataSize;
                        std::array<boost::asio::mutable_buffer, 6> buffersToFill = {
                            {{&receivedPacketChecksum, sizeof(receivedPacketChecksum)},
                             {receivedConnectionUuid.data, receivedConnectionUuid.size()},
                             {&receivedFlags, sizeof(receivedFlags)},
                             {&receivedPacketId, sizeof(receivedPacketId)},
                             {&receivedAckNextExpectedPacketId, sizeof(receivedAckNextExpectedPacketId)},
                             {&receivedDataSize, sizeof(receivedDataSize)}}};

                        std::array<unsigned char, ReliableUdpConnection::packetHeaderSize> packetHeaderBytes;
                        std::copy(individualConnection->second.bufferedBytes.begin(),
                                  individualConnection->second.bufferedBytes.begin() +
                                      ReliableUdpConnection::packetHeaderSize,
                                  packetHeaderBytes.begin());
                        auto asioBufferCopiedByteCount = boost::asio::buffer_copy(
                            buffersToFill,
                            boost::asio::const_buffer(
                                &packetHeaderBytes.at(ReliableUdpConnection::packetBeginMagic.size()),
                                packetHeaderBytes.size() - ReliableUdpConnection::packetBeginMagic.size()));
                        if (asioBufferCopiedByteCount != (ReliableUdpConnection::packetHeaderSize -
                                                          ReliableUdpConnection::packetBeginMagic.size())) {
                            throw std::logic_error("wtf ??? (somehow we could not fill all the buffers with the packet "
                                                   "header - this should never happen)");
                        }
                        boost::endian::little_to_native_inplace(receivedPacketChecksum);
                        boost::endian::little_to_native_inplace(receivedPacketId);
                        boost::endian::little_to_native_inplace(receivedAckNextExpectedPacketId);
                        boost::endian::little_to_native_inplace(receivedDataSize);
                        bool isInitialPacket        = static_cast<bool>(receivedFlags & 1);
                        bool isAcknowledgmentPacket = static_cast<bool>(receivedFlags & 2);
                        bool isLastPacket           = static_cast<bool>(receivedFlags & 4);

                        ReliableUdpConnection::our_crc64_type crc64_checksum;
                        crc64_checksum.process_block(packetHeaderBytes.begin() +
                                                         ReliableUdpConnection::packetBeginMagic.size() +
                                                         sizeof(std::uint64_t),
                                                     packetHeaderBytes.end());
                        for (std::size_t i = ReliableUdpConnection::packetHeaderSize;
                             i < ReliableUdpConnection::packetHeaderSize + receivedDataSize; ++i)
                            crc64_checksum.process_byte(individualConnection->second.bufferedBytes.at(i));

                        // Packet is corrupt or has incorrect UUID ! Best thing we can do is just drop the first byte of
                        // the buffer so that next time we look at the rest of the data... (note: we continue looking
                        // from here in case we find another packet after this in the remaining buffered data)
                        if (crc64_checksum.checksum() != receivedPacketChecksum ||
                            (individualConnection->second.uuid.has_value() &&
                             (*individualConnection->second.uuid != receivedConnectionUuid)) ||
                            (!individualConnection->second.uuid.has_value() && !isInitialPacket)) {
                            individualConnection->second.bufferedBytes.erase(
                                individualConnection->second.bufferedBytes.begin());
                            if (gReliableUdpConnectionDebugPrints)
                                std::cerr << std::this_thread::get_id() << " packet corrupt or has incorrect UUID\n";
                            continue;
                        }

                        // Handle last packet
                        if (isLastPacket) {
                            individualConnection->second.isConnectionDead = true;
                            break;
                        }

                        // Handle initial packet
                        if (isInitialPacket) {
                            if (gReliableUdpConnectionDebugPrints)
                                std::cerr << std::this_thread::get_id() << " Handling initial packet\n";
                            if (individualConnection->second.uuid.has_value() &&
                                receivedConnectionUuid != individualConnection->second.uuid.value())
                                throw std::logic_error(
                                    "individualConnection->second.uuid.has_value() just never should be true, wtf");
                            individualConnection->second.uuid.emplace(receivedConnectionUuid);
                            this->rawSendPacket({}, individualConnection->first,
                                                individualConnection->second.uuid.value(), false, true, false, 1, 0);
                        }

                        // Handle ACK packet
                        if (isAcknowledgmentPacket) {
                            if (gReliableUdpConnectionDebugPrints)
                                std::cerr << std::this_thread::get_id() << " Handling ACK " << receivedPacketId
                                          << " with next packet id " << receivedAckNextExpectedPacketId << '\n';
                            if (receivedAckNextExpectedPacketId >
                                individualConnection->second.remoteNextExpectedPacketId) {
                                individualConnection->second.remoteNextExpectedPacketId =
                                    receivedAckNextExpectedPacketId;
                                individualConnection->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.erase(
                                    individualConnection->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId
                                        .begin(),
                                    individualConnection->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId
                                        .lower_bound(receivedAckNextExpectedPacketId + 1));
                            }
                            if (receivedPacketId > individualConnection->second.remoteNextExpectedPacketId)
                                individualConnection->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.insert(
                                    receivedPacketId);
                        }

                        // Handle normal packet
                        if (receivedDataSize != 0) {
                            if (gReliableUdpConnectionDebugPrints)
                                std::cerr << std::this_thread::get_id() << " Handling normal packet "
                                          << receivedPacketId << '\n';
                            if (individualConnection->second.bufferedBytes.size() <
                                (ReliableUdpConnection::packetHeaderSize + receivedDataSize))
                                throw std::logic_error(
                                    "Somehow individualConnection->second.bufferedBytes.size() < "
                                    "(ReliableUdpConnection::packetHeaderSize + receivedDataSize) was true ????");
                            if (individualConnection->second.nextUnreadPacketId <= receivedPacketId) {
                                auto newPacket =
                                    individualConnection->second.availablePackets.try_emplace(receivedPacketId);
                                if (newPacket.second)
                                    newPacket.first->second.insert(
                                        newPacket.first->second.begin(),
                                        individualConnection->second.bufferedBytes.begin() +
                                            (ReliableUdpConnection::packetHeaderSize),
                                        individualConnection->second.bufferedBytes.begin() +
                                            (ReliableUdpConnection::packetHeaderSize + receivedDataSize));
                            }

                            std::uint64_t ackNextExpectedPacketId =
                                std::max(individualConnection->second.nextUnreadPacketId,
                                         static_cast<std::uint64_t>(1)) -
                                1;
                            for (auto &i : individualConnection->second.availablePackets)
                                if (i.first == ackNextExpectedPacketId + 1)
                                    ackNextExpectedPacketId = i.first;
                                else
                                    break;
                            ++ackNextExpectedPacketId;
                            this->rawSendPacket({}, individualConnection->first,
                                                individualConnection->second.uuid.value(), false, true, false,
                                                ackNextExpectedPacketId, receivedPacketId);
                        }

                        // Erase the packet we've processed from the connection buffer
                        individualConnection->second.bufferedBytes.erase(
                            individualConnection->second.bufferedBytes.begin(),
                            individualConnection->second.bufferedBytes.begin() +
                                ReliableUdpConnection::packetHeaderSize + receivedDataSize);
                    }
                }
                this->individualConnectionsCondVar.notify_all();
            }
        }
    }

    void beginReceiveThread()
    {
        std::call_once(this->beginReceiveThreadOnceFlag,
                       [this]() { this->receiveThread.emplace(([this] { this->receiveThreadFunction(); })); });
    }

public:
    ReliableUdpConnection() : socket(ioContext, boost::asio::ip::udp::v4()) {}

    ~ReliableUdpConnection()
    {
        if (gReliableUdpConnectionDebugPrints)
            std::cerr << "this->packetThreadsInFlight = " << this->packetThreadsInFlight << '\n';
        this->allThreadsShouldStop = true;
        this->individualConnectionsCondVar.notify_all();
        if (this->receiveThread.has_value())
            this->receiveThread->join();
        this->individualConnectionsCondVar.notify_all();
        std::this_thread::yield();
        while (this->packetThreadsInFlight != 0) {
            if (gReliableUdpConnectionDebugPrints)
                std::cerr << "this->packetThreadsInFlight = " << this->packetThreadsInFlight << '\n';
            this->individualConnectionsCondVar.notify_all();
            std::this_thread::yield();
        }
    }

    // This opens a new connection to the given host and port
    // Typically, this is used by clients to connect to a server
    boost::asio::ip::udp::endpoint openConnection(std::string_view host, std::uint16_t port)
    {
        boost::asio::ip::udp::resolver resolver(this->ioContext);

        // Set target endpoint and port.
        boost::asio::ip::udp::endpoint target_endpoint =
            *resolver.resolve(boost::asio::ip::udp::v4(), host, "").begin();
        target_endpoint.port(port);

        std::unique_lock<std::mutex> lockIndividualConnections(this->individualConnectionsMutex);

        if (this->individualConnections.find(target_endpoint) != this->individualConnections.end())
            throw std::runtime_error("Cannot simultaneously create several connections to the exact same peer");

        // Open socket.
        this->socket.connect(target_endpoint);
        this->beginReceiveThread();

        this->isReceiver = false;
        auto insertResult =
            this->individualConnections.insert(decltype(this->individualConnections)::value_type(target_endpoint, {}));

        if (!insertResult.second)
            throw std::runtime_error("Cannot simultaneously create several connections to the exact same peer (also "
                                     "wtf this error should have happened earlier)");

        insertResult.first->second.uuid.emplace(boost::uuids::random_generator()());

        std::size_t attempts = 0;
        do {
            this->rawSendPacket({}, target_endpoint, insertResult.first->second.uuid.value(), true, false, false, 0, 0);
            this->individualConnectionsCondVar.wait_for(lockIndividualConnections, std::chrono::milliseconds(50));
        } while (insertResult.first->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.find(0) ==
                     insertResult.first->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.end() &&
                 insertResult.first->second.remoteNextExpectedPacketId < 1 && attempts++ < 50);
        if (attempts >= 50)
            throw std::runtime_error("Opening connection failed: peer failed to respond within 2.5 seconds");

        return target_endpoint;
    }

    // This makes it so this ReliableUdpConnection is assigned to the given port
    // Typically, this is used by servers who want to receive data from arbitrary peers (i.e. manage an "infinite"
    // amount of connections)
    void prepareReceiving(std::uint16_t port)
    {
        this->isReceiver = true;
        this->socket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
        this->beginReceiveThread();
    }

    void sendPacket(const boost::asio::ip::udp::endpoint &endpoint, boost::asio::const_buffer data,
                    bool isBlocking = false)
    {
        if (data.size() > 4900000)
            throw std::runtime_error("Packets above 4.9 MB are not allowed (sorry)");
        std::uint64_t              sentPacketId;
        std::vector<unsigned char> dataCopy;
        dataCopy.insert(dataCopy.begin(), static_cast<const unsigned char *>(data.data()),
                        static_cast<const unsigned char *>(data.data()) + data.size());

        {
            std::unique_lock<std::mutex> connectionsLock(this->individualConnectionsMutex);

            auto connection = this->individualConnections.find(endpoint);
            if (connection == this->individualConnections.end())
                throw std::runtime_error("Cannot send packet on non-existent connection");

            if (connection->second.isConnectionDead)
                return;

            sentPacketId = connection->second.nextSentPacketId++;
            this->rawSendPacket(data, endpoint, connection->second.uuid.value(), false, false, false, 0, sentPacketId);
        }

        if (gReliableUdpConnectionDebugPrints)
            std::cerr << "Just started sending packet " << sentPacketId << '\n';

        this->packetThreadsInFlight++;
        if (this->allThreadsShouldStop) {
            this->packetThreadsInFlight--;
            return;
        }

        std::thread senderThread([this, endpoint, sentPacketId, dataCopyForThread = std::move(dataCopy)] {
            std::unique_lock<std::mutex> connectionsLock(this->individualConnectionsMutex);

            while (true) {
                auto connection = this->individualConnections.find(endpoint);
                // Conditions under which we just have to end ASAP
                if (connection == this->individualConnections.end() || this->allThreadsShouldStop ||
                    connection->second.isConnectionDead) {
                    this->packetThreadsInFlight--;
                    return;
                }
                if (connection->second.remoteNextExpectedPacketId > sentPacketId ||
                    connection->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.find(sentPacketId) !=
                        connection->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.end()) {
                    this->packetThreadsInFlight--;
                    return;
                }
                this->rawSendPacket(boost::asio::const_buffer(dataCopyForThread.data(), dataCopyForThread.size()),
                                    endpoint, connection->second.uuid.value(), false, false, false, 0, sentPacketId);
                if (gReliableUdpConnectionDebugPrints)
                    std::cerr << std::this_thread::get_id() << " Just sent raw packet for ID " << sentPacketId << '\n';
                this->individualConnectionsCondVar.wait_for(connectionsLock, std::chrono::milliseconds(30));
            }
        });

        if (isBlocking)
            senderThread.join();
        else
            senderThread.detach();
    }

    std::size_t receivePacket(boost::asio::ip::udp::endpoint &endpoint, boost::asio::mutable_buffer result,
                              bool isBlocking = true)
    {
        std::unique_lock<std::mutex> connectionsLock(this->individualConnectionsMutex);
        bool                         hasFinished = false;
        std::size_t                  receivedSize;

        while (true) {
            if (gReliableUdpConnectionDebugPrints)
                std::cerr << "Start attempt to receive packets\n";
            for (auto &i : this->individualConnections) {
                if (gReliableUdpConnectionDebugPrints)
                    std::cerr << "Attempting to receive packet " << i.second.nextUnreadPacketId << '\n';
                for (auto &j : i.second.availablePackets) {
                    if (j.first > i.second.nextUnreadPacketId)
                        break;
                    if (!j.second.empty()) {
                        receivedSize = boost::asio::buffer_copy(
                            result, boost::asio::const_buffer(j.second.data(), j.second.size()));
                        j.second.erase(j.second.begin(), j.second.begin() + receivedSize);
                        endpoint = i.first;
                        i.second.nextUnreadPacketId++;
                        hasFinished = true;
                        break;
                    }
                }
                while (!i.second.availablePackets.empty() &&
                       i.second.availablePackets.begin()->first < i.second.nextUnreadPacketId)
                    i.second.availablePackets.erase(i.second.availablePackets.begin());
                if (hasFinished) {
                    if (gReliableUdpConnectionDebugPrints)
                        std::cerr << "Successfully retrieved packet " << i.second.nextUnreadPacketId - 1 << '\n';
                    return receivedSize;
                }
            }
            if (!isBlocking)
                return 0;
            this->individualConnectionsCondVar.wait(connectionsLock);
        }
    }
};
