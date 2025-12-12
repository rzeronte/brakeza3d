//
// Created by Eduardo on 11/12/2025.
//

#include "../../include/Pools/PoolManager.h"

#include "../../include/Loaders/SceneLoader.h"

void PoolManager::ResolvePending()
{
    for (auto &p : pendingObjects) {
        p->function();
    }
    pendingObjects.clear();
}
