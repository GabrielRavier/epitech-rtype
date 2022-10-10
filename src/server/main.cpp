#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <boost/asio.hpp>
#include "network_manager.hpp"
#include "sync_queue.hpp"
#include "packet.hpp"

SynchronisedQueue<Packet> queue;

void NetworkLoop(uint16_t port)
{
    try {
        boost::asio::io_context io_context;
        ip::udp::socket         socket(io_context, ip::udp::endpoint(ip::udp::v4(), port));
        ip::udp::endpoint       remote_endpoint;

        // @REWORK IN PROGRESS : We probably have to maintain a list of "NetworkManager", one per user (connection) in a map.
        // Key is "source ip + source port" and value is "NetworkManager"
        while (true) {
            char *packet = new char[199];
            std::cout << "TEST." << std::endl;
            socket.receive_from(boost::asio::buffer(packet, 199), remote_endpoint);
        }

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
        start   = std::chrono::system_clock::now();

        std::cout << "TEST." << std::endl;

        const std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - start;
        const std::chrono::duration<double, std::milli> delta_ms((1000 / fps) - work_time.count());
        auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
    }
}

int main(int argc, char **argv)
{
    std::thread threadNetworkLoop(NetworkLoop, std::atoi(argv[1]));
    std::thread threadGameLoop(GameLoop);

    threadNetworkLoop.join();
    threadGameLoop.join();
}
