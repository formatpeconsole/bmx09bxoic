#pragma once
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <semaphore>
#include <vector>
#include <memory>
#include <functional>

class SimpleThreadPool
{
public:
    void init();
    void destroy();
    void waitWhenReady();
    void addTask(std::function<void()>&& f);
    bool shouldBreakThread();
private:
    std::mutex mtx{};
    std::condition_variable cv;
    std::counting_semaphore<> sm{ 0 };
    std::vector<std::thread> threads{};
    std::queue<std::function<void()>> tasks{};

    std::atomic_bool shouldQuit{ false };
    std::atomic_bool waiting{ false };
    std::atomic_int tasksCount{ 0 };
    int count{ 4 };
};

extern SimpleThreadPool& getSimpleThreadPoolInstance();