#include "../reliable-udp/ReliableUdpConnection.hpp"
#include <iostream>
#include <cassert>

int program()
{
    ReliableUdpConnection connectionServer;
    ReliableUdpConnection connectionClient;

    connectionServer.prepareReceiving(45678);
    auto endpoint = connectionClient.openConnection("localhost", 45678);

    std::array<char, 4> packet = {{1, 2, 3, 4}};
    connectionClient.sendPacket(endpoint, {packet.data(), packet.size()});

    packet = {5, 6, 7, 8};
    connectionClient.sendPacket(endpoint, {packet.data(), packet.size()});
    packet = {12, 13, 14, 15};
    connectionClient.sendPacket(endpoint, {packet.data(), packet.size()});

    std::array<char, 4> receivedPacket = {{0}};
    decltype(endpoint)  receiptEndpoint;
    auto sizeReceived = connectionServer.receivePacket(receiptEndpoint, {receivedPacket.data(), receivedPacket.size()});

    std::cerr << "Got first packet\n";
    assert(sizeReceived == 4);
    assert(receivedPacket[0] == 1);
    assert(receivedPacket[1] == 2);
    assert(receivedPacket[2] == 3);
    assert(receivedPacket[3] == 4);

    sizeReceived = connectionServer.receivePacket(receiptEndpoint, {receivedPacket.data(), receivedPacket.size()});

    assert(sizeReceived == 4);
    assert(receivedPacket[0] == 5);
    assert(receivedPacket[1] == 6);
    assert(receivedPacket[2] == 7);
    assert(receivedPacket[3] == 8);

    packet = {5, 6, 7, 3};
    connectionServer.sendPacket(receiptEndpoint, {packet.data(), packet.size()});

    sizeReceived = connectionServer.receivePacket(receiptEndpoint, {receivedPacket.data(), receivedPacket.size()});

    assert(sizeReceived == 4);
    assert(receivedPacket[0] == 12);
    assert(receivedPacket[1] == 13);
    assert(receivedPacket[2] == 14);
    assert(receivedPacket[3] == 15);

    receivedPacket.fill(0);
    sizeReceived = connectionClient.receivePacket(receiptEndpoint, {receivedPacket.data(), receivedPacket.size()});

    assert(sizeReceived == 4);
    assert(receivedPacket[0] == 5);
    assert(receivedPacket[1] == 6);
    assert(receivedPacket[2] == 7);
    assert(receivedPacket[3] == 3);

    return 0;
}

int main()
{
    try {
        return program();
    } catch (std::exception &exc) {
        std::cerr << "Error (stdexcept): " << exc.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Error: Unknown exception !!!!!!\n";
        return 1;
    }
}
