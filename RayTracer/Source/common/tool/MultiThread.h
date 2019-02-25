#pragma once

#include "../../ForwardDeclaration.h"
#include "../math/Vec2.h"
#include <thread>

namespace common
{
namespace tool
{

// TODO
// Ïß³Ì³Ø

static std::vector<std::thread> threads;
static thread_local int ThreadIndex;

int NumSystemCores()
{
    return (std::max)(1u, std::thread::hardware_concurrency());
}

int MaxThreadIndex()
{
    NumSystemCores();
}


void ParallelFor(std::function<void(int64_t)> func, int64_t count, int chunkSize = 1)
{
    CHECK(threads.size() > 0 || MaxThreadIndex() == 1);

    // Run iterations immediately if not using threads or if _count_ is small
    if (threads.empty() || count < chunkSize)
    {
        for (int64_t i = 0; i < count; ++i) func(i);
        return;
    }
    /* TODO
    // Create and enqueue _ParallelForLoop_ for this loop
    ParallelForLoop loop(std::move(func), count, chunkSize,
        CurrentProfilerState());
    workListMutex.lock();
    loop.next = workList;
    workList = &loop;
    workListMutex.unlock();

    // Notify worker threads of work to be done
    std::unique_lock<std::mutex> lock(workListMutex);
    workListCondition.notify_all();

    // Help out with parallel loop iterations in the current thread
    while (!loop.Finished())
    {
        // Run a chunk of loop iterations for _loop_

        // Find the set of loop iterations to run next
        int64_t indexStart = loop.nextIndex;
        int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);

        // Update _loop_ to reflect iterations this thread will run
        loop.nextIndex = indexEnd;
        if (loop.nextIndex == loop.maxIndex) workList = loop.next;
        loop.activeWorkers++;

        // Run loop indices in _[indexStart, indexEnd)_
        lock.unlock();
        for (int64_t index = indexStart; index < indexEnd; ++index)
        {
            uint64_t oldState = ProfilerState;
            ProfilerState = loop.profilerState;
            if (loop.func1D)
            {
                loop.func1D(index);
            }
            // Handle other types of loops
            else
            {
                CHECK(loop.func2D);
                loop.func2D(common::math::Vec2i(index % loop.nX, index / loop.nX));
            }
            ProfilerState = oldState;
        }
        lock.lock();

        // Update _loop_ to reflect completion of iterations
        loop.activeWorkers--;
    }
    */
}

void ParallelFor2D(std::function<void(common::math::Vec2i)> func, const common::math::Vec2i &count)
{
    CHECK(threads.size() > 0 || MaxThreadIndex() == 1);

    if (threads.empty() || count.x * count.y <= 1)
    {
        for (int y = 0; y < count.y; ++y)
            for (int x = 0; x < count.x; ++x) func(common::math::Vec2i(x, y));
        return;
    }

    /* TODO
    ParallelForLoop loop(std::move(func), count, CurrentProfilerState());
    {
        std::lock_guard<std::mutex> lock(workListMutex);
        loop.next = workList;
        workList = &loop;
    }

    std::unique_lock<std::mutex> lock(workListMutex);
    workListCondition.notify_all();

    // Help out with parallel loop iterations in the current thread
    while (!loop.Finished())
    {
        // Run a chunk of loop iterations for _loop_

        // Find the set of loop iterations to run next
        int64_t indexStart = loop.nextIndex;
        int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);

        // Update _loop_ to reflect iterations this thread will run
        loop.nextIndex = indexEnd;
        if (loop.nextIndex == loop.maxIndex) workList = loop.next;
        loop.activeWorkers++;

        // Run loop indices in _[indexStart, indexEnd)_
        lock.unlock();
        for (int64_t index = indexStart; index < indexEnd; ++index)
        {
            uint64_t oldState = ProfilerState;
            ProfilerState = loop.profilerState;
            if (loop.func1D)
            {
                loop.func1D(index);
            }
            // Handle other types of loops
            else
            {
                CHECK(loop.func2D);
                loop.func2D(common::math::Vec2i(index % loop.nX, index / loop.nX));
            }
            ProfilerState = oldState;
        }
        lock.lock();

        // Update _loop_ to reflect completion of iterations
        loop.activeWorkers--;
    }
    */
}


}
}