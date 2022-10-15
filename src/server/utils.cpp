#include "utils.hpp"
#include <chrono>

uint64_t getCurrentTimeEpoch()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
