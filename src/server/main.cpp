#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <map>
#include <string>
#include <boost/asio.hpp>
#include "core/NetworkServerManager.hpp"

void NetworkLoop(NetworkServerManager *networkManager)
{
    networkManager->run();
}

void GameLoop(uint16_t port)
{
    NetworkServerManager networkManager(port);
    std::thread threadNetworkLoop(NetworkLoop, &networkManager);
    bool                                  running = true;
    const double                          fps     = 60;
    std::chrono::system_clock::time_point start;

    while (running) {
        start = std::chrono::system_clock::now();

        // Process network packets.
        networkManager.processPackets();

        const std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - start;
        const std::chrono::duration<double, std::milli> delta_ms((1000 / fps) - work_time.count());
        auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
    }

    networkManager.close();
    threadNetworkLoop.join();
}

int main(int argc, char **argv)
{
    // Check arguments.
    if (argc < 2) {
        std::cout << "Try: " << argv[0] << " [port]." << std::endl;
        return (0);
    }

    // Start game server.
    GameLoop(std::atoi(argv[1]));

    return (0);
}
