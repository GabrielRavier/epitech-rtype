#include "ReliableUdpConnection.hpp"
#include <boost/asio/error.hpp>
#include <boost/system/detail/error_code.hpp>
#include <chrono>
#include <mutex>

static constexpr bool gReliableUdpConnectionDebugPrints = false;

bool ReliableUdpConnection::rawSendPacket(boost::asio::const_buffer             data,
                                          const boost::asio::ip::udp::endpoint &target_endpoint,
                                          const boost::uuids::uuid &connectionUuid, bool isInitialPacket,
                                          bool isAcknowledgmentPacket, bool isLastPacket,
                                          std::uint64_t ackNextExpectedPacketId, std::uint64_t packetId)
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
    boost::system::error_code send_error;
    this->socket.send_to(buffersToSend, target_endpoint, {}, send_error);
    if (gReliableUdpConnectionDebugPrints)
        std::cerr << std::this_thread::get_id() << " this->socket.send_to executed "
                  << (send_error.failed() ? "un-" : "") << "successfully for packet ID " << packetId << " and isAck "
                  << isAcknowledgmentPacket << " and ackNextExpectedPacketId " << ackNextExpectedPacketId << '\n';
    return !send_error.failed();
}

void ReliableUdpConnection::receiveThreadFunction()
{
    if (gReliableUdpConnectionDebugPrints)
        std::cerr << "Starting receive thread\n";
    std::vector<unsigned char> receiveBuffer;
    receiveBuffer.resize(50000);

    std::thread ioContextThread([this] {
        while (!this->allThreadsShouldStop) {
            if (this->ioContext.stopped())
                this->ioContext.restart();
            this->ioContext.run_for(std::chrono::milliseconds(50));
        }
    });

    std::mutex                     mutForSem;
    std::condition_variable        sem;
    std::unique_lock<std::mutex>   lockForSem(mutForSem);
    boost::system::error_code      receiveErrorCodeResult;
    std::size_t                    receiveReadBytesResult;
    boost::asio::ip::udp::endpoint endpoint;

    enum class ReceiveFromState
    {
        notStarted,
        unfinished,
        finished,
    };
    std::atomic<ReceiveFromState> receiveFromFinished = ReceiveFromState::notStarted;
    while (true) {

        if (receiveFromFinished == ReceiveFromState::notStarted) {
            receiveFromFinished = ReceiveFromState::unfinished;
            this->socket.async_receive_from(
                boost::asio::mutable_buffer(receiveBuffer.data(), receiveBuffer.size()), endpoint,
                [&receiveErrorCodeResult, &receiveReadBytesResult, &sem, &mutForSem,
                 &receiveFromFinished](const boost::system::error_code &errorCode, std::size_t readBytes) {
                    receiveErrorCodeResult = errorCode;
                    receiveReadBytesResult = readBytes;

                    std::lock_guard lockSem(mutForSem);
                    receiveFromFinished = ReceiveFromState::finished;
                    sem.notify_all();
                });
        }

        sem.wait_for(lockForSem, std::chrono::milliseconds(20));

        if (this->allThreadsShouldStop) {
            if (gReliableUdpConnectionDebugPrints)
                std::cerr << std::this_thread::get_id() << " receiver thread finishing up\n";
            lockForSem.unlock();
            ioContextThread.join();
            return;
        }
        if (receiveFromFinished == ReceiveFromState::finished) {
            receiveFromFinished = ReceiveFromState::notStarted;
            if (gReliableUdpConnectionDebugPrints)
                std::cerr << std::this_thread::get_id() << " this->socket.async_receive_from returned something !\n";
            if (receiveReadBytesResult > receiveBuffer.size())
                throw std::logic_error("wtf ???? receive_from should NEVER return higher than size just wtf");

            // If the endpoint we were given is 0, there's probably a problem...
            if (endpoint.port() == 0) {
                if (gReliableUdpConnectionDebugPrints)
                    std::cerr << std::this_thread::get_id() << " Receiver: Got endpoint with port 0\n";
                continue;
            }

            {
                std::lock_guard guardIndividualConnections(this->individualConnectionsMutex);

                auto individualConnection = this->individualConnections.find(endpoint);
                if (individualConnection == this->individualConnections.end()) {
                    if (!this->isReceiver)
                        continue;
                    individualConnection = this->individualConnections
                                               .insert(decltype(this->individualConnections)::value_type(endpoint, {}))
                                               .first;
                }
                individualConnection->second.lastPacketTime = std::chrono::steady_clock::now();
                if (receiveErrorCodeResult.failed() &&
                    receiveErrorCodeResult != boost::asio::error::operation_aborted) {
                    individualConnection->second.killThisConnection();
                    break;
                }
                individualConnection->second.isConnectionDead = false;
                individualConnection->second.bufferedBytes.insert(individualConnection->second.bufferedBytes.end(),
                                                                  receiveBuffer.begin(),
                                                                  receiveBuffer.begin() + receiveReadBytesResult);

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
                    individualConnection->second.bufferedBytes.erase(individualConnection->second.bufferedBytes.begin(),
                                                                     magicLocation);
                    if (individualConnection->second.bufferedBytes.at(0) != 0x7F)
                        throw std::logic_error(
                            "We found a packet magic and still individualConnection->second.bufferedBytes.at(0) != "
                            "0x7F is false ??");

                    // Is the header buffered ?
                    if (individualConnection->second.bufferedBytes.size() < ReliableUdpConnection::packetHeaderSize) {
                        if (gReliableUdpConnectionDebugPrints)
                            std::cerr << std::this_thread::get_id() << " header not buffered yet\n";
                        break;
                    }

                    // If the packet size is above 5 MB, something has gone terribly wrong (likely corruption) as we
                    // don't accept stuff that big. Just ignore this packet
                    std::uint64_t earlyReceivedDataSize;
                    std::copy(individualConnection->second.bufferedBytes.begin() +
                                  (ReliableUdpConnection::packetHeaderSize - 8),
                              individualConnection->second.bufferedBytes.begin() +
                                  (ReliableUdpConnection::packetHeaderSize),
                              reinterpret_cast<unsigned char *>(&earlyReceivedDataSize));
                    boost::endian::little_to_native_inplace(earlyReceivedDataSize);
                    if (earlyReceivedDataSize > 5000000) {
                        individualConnection->second.bufferedBytes.erase(
                            individualConnection->second.bufferedBytes.begin());
                        if (gReliableUdpConnectionDebugPrints)
                            std::cerr << std::this_thread::get_id() << " packet was too large\n";
                        continue;
                    }

                    // Is the whole packet buffered ?
                    if (individualConnection->second.bufferedBytes.size() <
                        ReliableUdpConnection::packetHeaderSize + earlyReceivedDataSize) {
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
                        buffersToFill, boost::asio::const_buffer(
                                           &packetHeaderBytes.at(ReliableUdpConnection::packetBeginMagic.size()),
                                           packetHeaderBytes.size() - ReliableUdpConnection::packetBeginMagic.size()));
                    if (asioBufferCopiedByteCount !=
                        (ReliableUdpConnection::packetHeaderSize - ReliableUdpConnection::packetBeginMagic.size())) {
                        throw std::logic_error("wtf ??? (somehow we could not fill all the buffers with the packet "
                                               "header - this should never happen)");
                    }
                    boost::endian::little_to_native_inplace(receivedPacketChecksum);
                    boost::endian::little_to_native_inplace(receivedPacketId);
                    boost::endian::little_to_native_inplace(receivedAckNextExpectedPacketId);
                    boost::endian::little_to_native_inplace(receivedDataSize);
                    assert(receivedDataSize == earlyReceivedDataSize);
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
                    if (isLastPacket)
                        individualConnection->second.killThisConnection();
                    else if (isInitialPacket) { // Handle initial packet
                        if (gReliableUdpConnectionDebugPrints)
                            std::cerr << std::this_thread::get_id() << " Handling initial packet\n";
                        if (individualConnection->second.uuid.has_value() &&
                            receivedConnectionUuid != individualConnection->second.uuid.value())
                            throw std::logic_error(
                                "individualConnection->second.uuid.has_value() just never should be true, wtf");
                        individualConnection->second.uuid.emplace(receivedConnectionUuid);
                        for (int i = 0; i < 5; ++i)
                            if (this->rawSendPacket({}, individualConnection->first,
                                                    individualConnection->second.uuid.value(), false, true, false, 1,
                                                    0))
                                break;
                    } else if (isAcknowledgmentPacket) { // Handle ACK packet
                        if (gReliableUdpConnectionDebugPrints)
                            std::cerr << std::this_thread::get_id() << " Handling ACK " << receivedPacketId
                                      << " with next packet id " << receivedAckNextExpectedPacketId << '\n';
                        if (receivedAckNextExpectedPacketId > individualConnection->second.remoteNextExpectedPacketId) {
                            individualConnection->second.remoteNextExpectedPacketId = receivedAckNextExpectedPacketId;
                            individualConnection->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.erase(
                                individualConnection->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.begin(),
                                individualConnection->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId
                                    .lower_bound(receivedAckNextExpectedPacketId));
                        }
                        if (receivedPacketId > individualConnection->second.remoteNextExpectedPacketId)
                            individualConnection->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.insert(
                                receivedPacketId);
                    } else if (receivedDataSize != 0) { // Handle normal packet
                        if (gReliableUdpConnectionDebugPrints)
                            std::cerr << std::this_thread::get_id() << " Handling normal packet " << receivedPacketId
                                      << '\n';
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
                            std::max(individualConnection->second.nextUnreadPacketId, static_cast<std::uint64_t>(1)) -
                            1;
                        for (auto &i : individualConnection->second.availablePackets)
                            if (i.first == ackNextExpectedPacketId + 1)
                                ackNextExpectedPacketId = i.first;
                            else
                                break;
                        ++ackNextExpectedPacketId;
                        for (int i = 0; i < 5; ++i)
                            if (this->rawSendPacket({}, individualConnection->first,
                                                    individualConnection->second.uuid.value(), false, true, false,
                                                    ackNextExpectedPacketId, receivedPacketId))
                                break;
                    }

                    // Erase the packet we've processed from the connection buffer
                    individualConnection->second.bufferedBytes.erase(
                        individualConnection->second.bufferedBytes.begin(),
                        individualConnection->second.bufferedBytes.begin() +
                            std::min(ReliableUdpConnection::packetHeaderSize + receivedDataSize,
                                     individualConnection->second.bufferedBytes.size()));
                }
            }
            this->individualConnectionsCondVar.notify_all();
        }
    }
}

void ReliableUdpConnection::beginReceiveThread()
{
    std::call_once(this->beginReceiveThreadOnceFlag,
                   [this]() { this->receiveThread.emplace(([this] { this->receiveThreadFunction(); })); });
}

ReliableUdpConnection::~ReliableUdpConnection() // NOLINT(bugprone-exception-escape)
{
    this->close();
}

boost::asio::ip::udp::endpoint ReliableUdpConnection::openConnection(std::string_view host, std::uint16_t port)
{
    if (gReliableUdpConnectionDebugPrints)
        std::cerr << "Opening connection towards " << host << ":" << port << '\n';
    boost::asio::ip::udp::resolver resolver(this->ioContext);

    // Set target endpoint and port.
    boost::asio::ip::udp::endpoint target_endpoint = *resolver.resolve(boost::asio::ip::udp::v4(), host, "").begin();
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

    auto connIterator = insertResult.first;
    connIterator->second.uuid.emplace(boost::uuids::random_generator()());

    std::size_t attempts = 0;
    do {
        if (connIterator->second.isConnectionDead)
            break;
        if (!this->rawSendPacket({}, target_endpoint, connIterator->second.uuid.value(), true, false, false, 0, 0))
            attempts += 5;
        this->individualConnectionsCondVar.wait_for(lockIndividualConnections, std::chrono::milliseconds(50));
        connIterator = this->individualConnections.find(target_endpoint);
    } while (connIterator != this->individualConnections.end() &&
             connIterator->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.find(0) ==
                 connIterator->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.end() &&
             connIterator->second.remoteNextExpectedPacketId < 1 && attempts++ < 50);
    if (attempts >= 50 || connIterator == this->individualConnections.end() || connIterator->second.isConnectionDead)
        throw std::runtime_error("Opening connection failed: peer failed to respond within 2.5 seconds or immediately "
                                 "terminated the connection");

    return target_endpoint;
}

void ReliableUdpConnection::prepareReceiving(std::uint16_t port)
{
    this->isReceiver = true;
    this->socket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
    this->beginReceiveThread();
}

void ReliableUdpConnection::sendPacket(const boost::asio::ip::udp::endpoint &endpoint, boost::asio::const_buffer data,
                                       bool isBlocking)
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
        if (connection == this->individualConnections.end() || !connection->second.uuid.has_value())
            throw std::runtime_error("Cannot send packet on non-existent connection");

        connection->second.checkLastPacket();
        if (connection->second.isConnectionDead)
            return;

        sentPacketId = connection->second.nextSentPacketId++;
        if (connection->second.remoteNextExpectedPacketId + 500 < sentPacketId) {
            bool hadError = false;
            for (int i = 0; i < 5; ++i)
                if (!this->rawSendPacket(data, endpoint, connection->second.uuid.value(), false, false, false, 0,
                                         sentPacketId))
                    hadError = true;
            if (hadError)
                return;
        }
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

        std::size_t                           attempts     = 0;
        std::chrono::steady_clock::time_point lastSendTime = std::chrono::steady_clock::now();
        while (true) {
            auto connection = this->individualConnections.find(endpoint);

            if (connection != this->individualConnections.end())
                connection->second.checkLastPacket();

            // Conditions under which we just have to end ASAP
            if (connection == this->individualConnections.end() || this->allThreadsShouldStop ||
                connection->second.isConnectionDead || !connection->second.uuid.has_value()) {
                this->packetThreadsInFlight--;
                return;
            }

            // Check if packet has been received yet
            if (connection->second.remoteNextExpectedPacketId > sentPacketId ||
                connection->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.find(sentPacketId) !=
                    connection->second.remoteAcknowledgedPacketsAboveNextExpectedPacketId.end()) {
                this->packetThreadsInFlight--;
                return;
            }

            if ((connection->second.remoteNextExpectedPacketId + 300) > sentPacketId &&
                lastSendTime + std::chrono::milliseconds(
                                   std::min(5 * (sentPacketId - connection->second.remoteNextExpectedPacketId),
                                            static_cast<std::uint64_t>(50000))) <
                    std::chrono::steady_clock::now()) {
                for (int i = 0; i < 5; ++i)
                    if (!this->rawSendPacket(
                            boost::asio::const_buffer(dataCopyForThread.data(), dataCopyForThread.size()), endpoint,
                            connection->second.uuid.value(), false, false, false, 0, sentPacketId))
                        return;
                lastSendTime = std::chrono::steady_clock::now();
            }
            auto waitTime = std::chrono::milliseconds(1);
            if (connection->second.remoteNextExpectedPacketId + 30 < sentPacketId) {
                waitTime *= 3;
                if (this->packetThreadsInFlight > 30)
                    waitTime *= 3;
            }
            if (connection->second.remoteNextExpectedPacketId + 90 < sentPacketId) {
                waitTime *= 3;
                if (this->packetThreadsInFlight > 90)
                    waitTime *= 3;
            }
            waitTime = std::min(waitTime, std::chrono::milliseconds(1000));

            auto sleepUntilTime = std::chrono::steady_clock::now() + waitTime;
            this->individualConnectionsCondVar.wait_for(connectionsLock, waitTime);
            connectionsLock.unlock();
            {
                std::unique_lock<std::mutex> allThreadsShouldStopNotificationMutexLock(
                    this->allThreadsShouldStopNotificationMutex);
                this->allThreadShouldStopNotification.wait_until(allThreadsShouldStopNotificationMutexLock,
                                                                 sleepUntilTime);
            }
            connectionsLock.lock();
            ++attempts;
        }
    });

    if (isBlocking)
        senderThread.join();
    else
        senderThread.detach();
}

std::size_t ReliableUdpConnection::receivePacket(boost::asio::ip::udp::endpoint &endpoint,
                                                 boost::asio::mutable_buffer result, bool isBlocking)
{
    std::unique_lock<std::mutex> connectionsLock(this->individualConnectionsMutex);
    bool                         hasFinished = false;
    std::size_t                  receivedSize;

    ++this->packetThreadsInFlight;
    while (true) {
        if (this->allThreadsShouldStop)
            break;
        if (gReliableUdpConnectionDebugPrints)
            std::cerr << "Start attempt to receive packets\n";
        for (auto &i : this->individualConnections) {
            i.second.checkLastPacket();
            if (i.second.isConnectionDead)
                continue;
            if (gReliableUdpConnectionDebugPrints)
                std::cerr << "Attempting to receive packet " << i.second.nextUnreadPacketId << '\n';
            for (auto &j : i.second.availablePackets) {
                if (j.first > i.second.nextUnreadPacketId)
                    break;
                if (!j.second.empty()) {
                    receivedSize =
                        boost::asio::buffer_copy(result, boost::asio::const_buffer(j.second.data(), j.second.size()));
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
                --this->packetThreadsInFlight;
                return receivedSize;
            }
        }
        if (!isBlocking)
            break;
        this->individualConnectionsCondVar.wait(connectionsLock);
    }
    --this->packetThreadsInFlight;
    return 0;
}

bool ReliableUdpConnection::isOpen()
{
    std::lock_guard guard(this->individualConnectionsMutex);

    return this->socket.is_open();
}

void ReliableUdpConnection::close()
{
    {
        std::lock_guard<std::mutex> guard(this->individualConnectionsMutex);

        if (this->socket.is_open())
            for (auto &i : this->individualConnections)
                if (!i.second.isConnectionDead && i.second.uuid.has_value()) {
                    for (int j = 0; j < 10; ++j)
                        this->rawSendPacket({}, i.first, *i.second.uuid, false, false, true, 0, 0);
                }
    }
    if (gReliableUdpConnectionDebugPrints)
        std::cerr << "this->packetThreadsInFlight = " << this->packetThreadsInFlight << '\n';
    this->allThreadsShouldStop = true;
    this->individualConnectionsCondVar.notify_all();
    this->allThreadShouldStopNotification.notify_all();
    if (this->receiveThread.has_value()) {
        this->receiveThread->join();
        this->receiveThread.reset();
    }
    this->individualConnectionsCondVar.notify_all();
    std::this_thread::yield();
    while (this->packetThreadsInFlight != 0) {
        if (gReliableUdpConnectionDebugPrints)
            std::cerr << "this->packetThreadsInFlight = " << this->packetThreadsInFlight << '\n';
        this->individualConnectionsCondVar.notify_all();
        this->allThreadShouldStopNotification.notify_all();
        std::this_thread::yield();
    }

    this->socket.close();
}
