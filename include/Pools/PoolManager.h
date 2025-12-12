//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_POOLMANAGER_H
#define BRAKEZA3D_POOLMANAGER_H
#include "ThreadPool.h"

class PoolManager {

    ThreadPool jobsPool;

    ThreadPool ioPool;
    ThreadPool computePool;
    std::vector<PendingJob*> pendingObjects;

public:
    [[nodiscard]] ThreadPool & getIOPool() { return ioPool; }
    [[nodiscard]] ThreadPool & getJobsPool() { return jobsPool; };
    [[nodiscard]] ThreadPool & getComputePool() { return computePool; };
    [[nodiscard]] std::vector<PendingJob*> & getPendingJobs() { return pendingObjects; };

    void ResolvePending();
};


#endif //BRAKEZA3D_POOLMANAGER_H