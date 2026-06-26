#include "../../include/Render/ThreadPool.h"
#include <chrono>
#include <iostream>

void ThreadPool::spawnWorkers(size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::shared_ptr<ThreadJobBase> job;

                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this] {
                        return stop ||
                               (!tasks.empty() && activeTasks < (int)maxConcurrentTasks);
                    });

                    if (stop && tasks.empty())
                        return;

                    if (activeTasks >= (int)maxConcurrentTasks)
                        continue;

                    job = std::move(tasks.front());
                    tasks.pop();
                }

                cont++;
                activeTasks++;

                try {
                    job->function();
                    job->callback();
                } catch (const std::exception& e) {
                    std::cerr << "[ThreadPool] Job exception: " << e.what() << std::endl;
                } catch (...) {
                    std::cerr << "[ThreadPool] Job unknown exception" << std::endl;
                }

                activeTasks--;
                condition.notify_one();
            }
        });
    }
}

ThreadPool::ThreadPool(size_t numThreads)
:
    stop(false),
    activeTasks(0),
    cont(0),
    maxCallbacksPerFrame(5),
    maxConcurrentTasks(4),
    maxEnqueuedTasks(4096),
    maxEnqueuedCallbacks(4096)
{
    spawnWorkers(numThreads);
}

ThreadPool::~ThreadPool() {
    stop = true;
    condition.notify_all();
    for (std::thread& worker : workers)
        worker.join();
}

void ThreadPool::enqueue(std::shared_ptr<ThreadJobBase> job) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        // Prevenir saturación de cola
        if (tasks.size() >= maxEnqueuedTasks) {
            std::cerr << "[ThreadPool] Queue full (" << maxEnqueuedTasks
                      << "), dropping task" << std::endl;
            // O usar Logging::Warning si lo tienes
            return;
        }

        tasks.push(job);
    }
    condition.notify_one();
}

void ThreadPool::enqueueWithMainThreadCallback(std::shared_ptr<ThreadJobBase> job) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        // Prevenir saturación de cola
        if (tasks.size() >= maxEnqueuedTasks) {
            std::cerr << "[ThreadPool] Queue full (" << maxEnqueuedTasks
                      << "), dropping task" << std::endl;
            // O usar Logging::Warning si lo tienes
            return;
        }

        // Crear wrapper job que ejecuta function en worker thread
        // y encola callback para main thread
        auto workerJob = std::make_shared<ThreadJobBase>(
            job->function,
            [this, job]() {
                std::unique_lock<std::mutex> callbackLock(callbackMutex);

                if (mainThreadCallbacks.size() >= maxEnqueuedCallbacks) {
                    std::cerr << "[ThreadPool] Callback queue full (" << maxEnqueuedCallbacks
                              << "), dropping callback" << std::endl;
                    return;
                }

                mainThreadCallbacks.push([job]() {
                    job->callback();
                });
            }
        );

        tasks.push(workerJob);
    }
    condition.notify_one();
}

void ThreadPool::processMainThreadCallbacks() {
    std::unique_lock<std::mutex> lock(callbackMutex);

    size_t processed = 0;

    // Procesar solo maxCallbacksPerFrame callbacks por frame
    while (!mainThreadCallbacks.empty() && processed < maxCallbacksPerFrame) {
        auto callback = mainThreadCallbacks.front();
        mainThreadCallbacks.pop();

        // Liberar lock antes de ejecutar callback (puede ser lento)
        lock.unlock();

        try {
            callback();
        } catch (const std::exception& e) {
            std::cerr << "[ThreadPool] Callback exception: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "[ThreadPool] Callback unknown exception" << std::endl;
        }

        lock.lock();
        processed++;
    }

    // Debug opcional: avisar si quedan callbacks pendientes
    if (!mainThreadCallbacks.empty()) {
        // std::cout << "[ThreadPool] " << mainThreadCallbacks.size()
        //           << " callbacks pending for next frame" << std::endl;
    }
}

size_t ThreadPool::getPendingTasks() const {
    std::unique_lock<std::mutex> lock(queueMutex);
    return tasks.size();
}

size_t ThreadPool::getPendingCallbacks() const {
    std::unique_lock<std::mutex> lock(callbackMutex);
    return mainThreadCallbacks.size();
}

int ThreadPool::getActiveTasks() const {
    return activeTasks;
}

void ThreadPool::waitAll() {
    while (getPendingTasks() > 0 || getActiveTasks() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int ThreadPool::getCont()
{
    return (int)cont;
}

void ThreadPool::resize(size_t newNumThreads)
{
    if (newNumThreads == 0) newNumThreads = 1;

    waitAll();

    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();

    for (std::thread& worker : workers)
        worker.join();

    workers.clear();
    stop = false;

    spawnWorkers(newNumThreads);
}