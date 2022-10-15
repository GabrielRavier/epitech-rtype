#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <map>
#include <string>
#include <boost/asio.hpp>
#include "network_manager.hpp"
#include "sync_queue.hpp"
#include "packets/packet.hpp"

SynchronisedQueue<Packet> queue;
std::map<std::string, NetworkManager> managers;

void NetworkLoop(uint16_t port)
{
    try {
        uint8_t buf[4096];
        boost::asio::mutable_buffer buffer = boost::asio::buffer(buf, 4096);
        boost::asio::io_context io_context;
        ip::udp::socket         socket(io_context, ip::udp::endpoint(ip::udp::v4(), port));
        ip::udp::endpoint       remote_endpoint;

        while (socket.is_open()) {
            socket.receive_from(buffer, remote_endpoint);
            std::string address = remote_endpoint.address().to_string() + std::to_string(remote_endpoint.port());
                std::cout << address << std::endl;

            if (managers.find(address) == managers.end()) {
                managers[address] = NetworkManager();


            } else {

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
