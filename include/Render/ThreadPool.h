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
    std::atomic<int> cont;

public:
    explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    ~ThreadPool();

    // Encolar job con callback en worker thread
    void enqueue(std::shared_ptr<ThreadJobBase> job);

    // Encolar job con callback diferido al main thread
    void enqueueWithMainThreadCallback(std::shared_ptr<ThreadJobBase> job);

    // Procesar callbacks en main thread
    void processMainThreadCallbacks();

    size_t getPendingTasks() const;
    size_t getPendingCallbacks() const;
    int getActiveTasks() const;
    void waitAll();

    int getCont();
};

#endif //BRAKEZA3D_THREADPOOL_H