#pragma once
#include <mutex>
#include <chrono>

struct ClockInstance
{
    std::atomic<std::chrono::steady_clock::time_point> now;

    void updateTime();
    std::chrono::steady_clock::time_point getTime();
};

extern ClockInstance& getClockInstance();