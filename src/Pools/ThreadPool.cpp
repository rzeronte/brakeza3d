#include "../../include/Pools/ThreadPool.h"
#include <chrono>

ThreadPool::ThreadPool(size_t numThreads)
:
    stop(false),
    activeTasks(0)
{
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::shared_ptr<PendingJob> job;  // ← Cambio aquí

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

                activeTasks++;
                job->function();  // ← Cambio: -> en lugar de .
                job->callback();  // ← Cambio: -> en lugar de .
                activeTasks--;
                // job se borra automáticamente aquí cuando sale de scope
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

void ThreadPool::enqueue(std::shared_ptr<PendingJob> job) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(job);
    }
    condition.notify_one();
}

void ThreadPool::enqueueWithMainThreadCallback(std::shared_ptr<PendingJob> job) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        // Crea un nuevo job que solo ejecuta la función
        // y guarda el callback para main thread
        auto workerJob = std::make_shared<PendingJob>(
            job->function,
            [this, callback = job->callback]() {
                std::unique_lock<std::mutex> callbackLock(callbackMutex);
                mainThreadCallbacks.push(callback);
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
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}