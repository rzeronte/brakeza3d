#include "../../include/Render/ThreadPool.h"
#include <chrono>
#include <iostream>

ThreadPool::ThreadPool(size_t numThreads)
:
    stop(false),
    activeTasks(0),
    cont(0)
{
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::shared_ptr<ThreadJobBase> job;

                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this] {
                        return stop || !tasks.empty();
                    });

                    if (stop && tasks.empty())
                        return;

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
            }
        });
    }
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
        tasks.push(job);
    }
    condition.notify_one();
}

void ThreadPool::enqueueWithMainThreadCallback(std::shared_ptr<ThreadJobBase> job) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        // Crear wrapper job que ejecuta function en worker thread
        // y encola callback para main thread
        auto workerJob = std::make_shared<ThreadJobBase>(
            job->function,
            [this, job]() {  // ← Capturar job para mantenerlo vivo
                std::unique_lock<std::mutex> callbackLock(callbackMutex);
                // ← Pushear lambda que captura job para mantenerlo vivo hasta callback
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
    while (!mainThreadCallbacks.empty()) {
        auto callback = mainThreadCallbacks.front();
        mainThreadCallbacks.pop();
        lock.unlock();
        callback();
        lock.lock();
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