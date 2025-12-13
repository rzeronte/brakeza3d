//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_POOLMANAGER_H
#define BRAKEZA3D_POOLMANAGER_H
#include "ThreadPool.h"

class PoolManager {

    ThreadPool ioPool;

public:
    [[nodiscard]] ThreadPool & Pool() { return ioPool; }

    void ResolvePending();
};


#endif //BRAKEZA3D_POOLMANAGER_H