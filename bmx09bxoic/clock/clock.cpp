#include "clock.h"

void ClockInstance::updateTime()
{
    now.store(std::chrono::high_resolution_clock::now(), std::memory_order_release);
}

std::chrono::steady_clock::time_point ClockInstance::getTime()
{
    return now.load(std::memory_order_acquire);
}

ClockInstance& getClockInstance()
{
    static ClockInstance ptr;
    return ptr;
}
