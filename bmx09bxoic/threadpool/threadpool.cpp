#include "threadpool.h"

#include <iostream>
#include <algorithm>

#include "../mem/raii.h"

void SimpleThreadPool::init()
{
    count = std::clamp<int>(std::thread::hardware_concurrency(), 2, 16);
    threads.reserve(count);

    for (int i = 0; i < count; ++i)
        threads.emplace_back(std::thread([this]()
            {
                while (true)
                {
                    std::function<void()> task{};
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait(lock, [this]() { return shouldQuit || !tasks.empty(); });

                        if (shouldQuit)
                            return;

                        task = tasks.front();
                        tasks.pop();

                        if (!waiting)
                            --tasksCount;
                    }

                    if (task)
                    {
                        task();
                        sm.release();
                    }
                }
            }));
}

void SimpleThreadPool::destroy()
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        shouldQuit = true;
        cv.notify_all();
    }

    for (auto& t : threads)
        t.join();

    threads.clear();
}

void SimpleThreadPool::waitWhenReady()
{
    SCOPE_SET(waiting, true, false);

    for (int i = 0; i < tasksCount; ++i)
        sm.acquire();

    tasksCount = 0;
}

void SimpleThreadPool::addTask(std::function<void()>&& f)
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        tasks.push(f);
        ++tasksCount;
    }
    cv.notify_one();
}

bool SimpleThreadPool::shouldBreakThread()
{
    return shouldQuit;
}

SimpleThreadPool& getSimpleThreadPoolInstance()
{
    static SimpleThreadPool ptr;
    return ptr;
}