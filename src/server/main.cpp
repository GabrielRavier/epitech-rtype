#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <map>
#include <string>
#include <boost/asio.hpp>
#include "network_manager.hpp"
#include "sync_queue.hpp"
#include "packets/connection_state.hpp"

SynchronisedQueue<Packet> queue;
std::map<std::string, NetworkManager> managers;

void NetworkLoop(uint16_t port)
{
    try {
        boost::asio::io_context io_context;
        ip::udp::socket         socket(io_context, ip::udp::endpoint(ip::udp::v4(), port));
        ip::udp::endpoint       remote_endpoint;
        Buffer buffer(4096);

        while (socket.is_open()) {
            try {
                size_t len = buffer.pos() + socket.receive_from(
                    boost::asio::buffer(buffer.data() + buffer.pos(), buffer.capacity() - buffer.pos()),
                    remote_endpoint
                );

                // Set buffer to begin.
                buffer.setPos(0);

                // Decode packets.
                while (buffer.pos() < len) {
                    uint16_t packet_len = buffer.readU16();

                    // Ensure packet size.
                    if ((packet_len + 2) > buffer.capacity())
                        throw std::exception("Too big packet.");

                    // Ensure packet entirety.
                    if ((buffer.pos() + packet_len) > len) {
                        buffer.setPos(buffer.pos() - 2);
                        break;
                    }

                    // Read packet header.
                    uint16_t seq_client = buffer.readU16();
                    uint16_t seq_server = buffer.readU16();
                    uint8_t packet_id   = buffer.readU8();
                    Packet *packet      = CreateClientPacket(packet_id);

                    // Read packet data.
                    packet->readPacket(&buffer);

                    // Enqueue packet.
                    // m_queue_in.enqueue(packet);
                    std::cerr << std::to_string(packet_id) << std::endl;
                    delete packet;
                }

                // Move remaining to front.
                buffer.moveToFront(len);

            } catch (std::exception &e) {
                std::cerr << "ERR: " << e.what() << std::endl;
            }
        }

        std::cerr << "DISCONNECTED." << std::endl;

    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void GameLoop()
{
    bool                                  running = true;
    const double                          fps     = 60;
    std::chrono::system_clock::time_point start;

    while (running) {
        start = std::chrono::system_clock::now();

        const std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - start;
        const std::chrono::duration<double, std::milli> delta_ms((1000 / fps) - work_time.count());
        auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
    }
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cout << "Try: " << argv[0] << " [port]." << std::endl;
        return (0);
    }

    std::thread threadNetworkLoop(NetworkLoop, std::atoi(argv[1]));
    std::thread threadGameLoop(GameLoop);

    threadNetworkLoop.join();
    threadGameLoop.join();
}
