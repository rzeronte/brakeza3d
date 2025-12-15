//
// Created by Eduardo on 10/12/2025.
//

#ifndef BRAKEZA3D_THREADPOOL_H
#define BRAKEZA3D_THREADPOOL_H

#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>
#include "../Threads/ThreadJobBase.h"

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::shared_ptr<ThreadJobBase>> tasks;
    std::queue<std::function<void()>> mainThreadCallbacks;

    mutable std::mutex queueMutex;
    mutable std::mutex callbackMutex;
    std::condition_variable condition;

    std::atomic<bool> stop;
    std::atomic<int> activeTasks;
    std::atomic<size_t> cont;

    size_t maxCallbacksPerFrame;
    size_t maxConcurrentTasks;
    size_t maxEnqueuedTasks;

public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    // No permitir copias
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Encolar trabajos
    void enqueue(std::shared_ptr<ThreadJobBase> job);
    void enqueueWithMainThreadCallback(std::shared_ptr<ThreadJobBase> job);

    // Procesar callbacks (llamar desde main thread cada frame)
    void processMainThreadCallbacks();

    // Configuración
    void setMaxCallbacksPerFrame(size_t max)    { maxCallbacksPerFrame = max; }
    void setMaxConcurrentTasks(size_t max)      { maxConcurrentTasks = max; }
    void setMaxEnqueuedTasks(size_t max)        { maxEnqueuedTasks = max; }

    size_t getMaxCallbacksPerFrame() const  { return maxCallbacksPerFrame; }
    size_t getMaxConcurrentTasks() const    { return maxConcurrentTasks; }
    size_t getMaxEnqueuedTasks() const      { return maxEnqueuedTasks; }

    // Información
    size_t getPendingTasks() const;
    size_t getPendingCallbacks() const;
    int getActiveTasks() const;
    int getCont();

    // Esperar a que termine todo
    void waitAll();
};

#endif //BRAKEZA3D_THREADPOOL_H