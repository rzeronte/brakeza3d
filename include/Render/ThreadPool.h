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
#include <cstdint>
#include <memory>
#include <string>
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

    // Generacion de escena actual. cancelPending() la incrementa; enqueueWithMainThreadCallback
    // captura la generacion vigente en el momento de encolar, y descarta (sin ejecutar) el
    // callback si para cuando le toca correr la generacion ya avanzo -- cubre el job que estaba
    // EN EJECUCION (no en cola) en el instante exacto de un restart, que cancelPending() no
    // puede tocar. Ver cancelPending().
    std::atomic<uint64_t> epoch;

    std::atomic<size_t> maxCallbacksPerFrame;
    std::atomic<size_t> maxConcurrentTasks;
    std::atomic<size_t> maxEnqueuedTasks;
    std::atomic<size_t> maxEnqueuedCallbacks;

    void spawnWorkers(size_t n);

    // Crea el wrapper que ejecuta job->function() en el worker y encola job->callback() al hilo
    // principal. Si el job sale con deferred=true, se reencola a sí mismo (misma epoch) en vez
    // de encolar el callback. Llamar con queueMutex tomado.
    std::shared_ptr<ThreadJobBase> makeMainThreadCallbackWrapper(std::shared_ptr<ThreadJobBase> job, uint64_t jobEpoch);

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
    void setMaxEnqueuedCallbacks(size_t max)    { maxEnqueuedCallbacks = max; }

    size_t getMaxCallbacksPerFrame() const  { return maxCallbacksPerFrame; }
    size_t getMaxConcurrentTasks() const    { return maxConcurrentTasks; }
    size_t getMaxEnqueuedTasks() const      { return maxEnqueuedTasks; }
    size_t getMaxEnqueuedCallbacks() const  { return maxEnqueuedCallbacks; }

    // Información
    size_t getPendingTasks() const;
    size_t getPendingCallbacks() const;
    int getActiveTasks() const;
    int getCont();

    // Esperar a que termine todo
    void waitAll();

    // Descarta toda tarea encolada aun no empezada y todo callback de main-thread aun no
    // procesado, y avanza la generacion (epoch) -- usar antes de un reinicio/cambio de escena
    // para no arrastrar trabajo de la escena anterior (ver Brakeza::cancelPendingJobs(),
    // llamado desde doRestart() en MenuManager.lua). Un job YA en ejecucion en ese instante
    // (como mucho maxConcurrentTasks en vuelo por pool) sigue corriendo, pero su callback queda
    // etiquetado con la generacion VIEJA y se descarta solo al llegarle el turno -- ver
    // enqueueWithMainThreadCallback().
    void cancelPending();

    // Redimensionar el pool (espera tareas activas, reinicia workers)
    void resize(size_t newNumThreads);

    size_t getNumThreads() const { return workers.size(); }
};

#endif //BRAKEZA3D_THREADPOOL_H