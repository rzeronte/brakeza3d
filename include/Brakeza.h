#ifndef BRAKEDA3D_BRAKEZA3D_H
#define BRAKEDA3D_BRAKEZA3D_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>
#include <mutex>
#include "Components/Component.h"
#include "GUI/GUIManager.h"
#include "Render/ThreadPool.h"

class Brakeza
{
    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;

    Components *componentsManager;
    Config::LineCommandOptions cliOptions;
    int exitCode = 0;
    Timer timer;

    std::vector<Object3D *> objects;
    mutable std::shared_mutex objectsMutex;
    std::unordered_map<std::string, Object3D *> objectsByName;
    std::unordered_map<unsigned int, Object3D *> objectsById;

    // Registro de punteros Object3D VIVOS ahora mismo. Object3D::Object3D() se registra,
    // Object3D::~Object3D() se borra como primerisima linea. isValidObjectPointer() es la
    // UNICA forma segura de comprobar un Object3D* que puede venir de un script Lua tras un
    // reinicio de partida: solo compara el VALOR del puntero contra este set, nunca lo
    // desreferencia -- por eso funciona aunque apunte a memoria ya liberada (o reutilizada por
    // otro objeto), a diferencia de obj==nullptr (no detecta colgantes) o obj->isRemoved()
    // (desreferencia -> el propio crash que se intenta evitar).
    // Mutex PROPIO, deliberadamente NO objectsMutex: ComponentRender::DeleteRemovedObjects()
    // mantiene uniqueLockObjects() (objectsMutex) durante el `delete object` que dispara
    // ~Object3D() -- std::shared_mutex no es reentrante, reusar objectsMutex aqui deadlockea
    // el motor entero en el primer objeto borrado (reproducido: proceso colgado, 0% CPU, sin
    // avanzar ningun frame mas). Un mutex independiente evita el problema sin tocar el locking
    // ya existente de objects/objectsByName/objectsById.
    mutable std::mutex livePointersMutex;
    std::unordered_set<Object3D *> livePointers;

    GUIManager managerGUI;
    ThreadPool pool;
    ThreadPool poolImages;

public:
    Brakeza();
    virtual ~Brakeza();

    bool ReadArgs(int argc, char **argv);
    void Start(int argc, char *argv[]);
    void MainLoop();
    void AddObject3D(Object3D *obj, const std::string &label);
    void UpdateTimer();
    void OnStartComponents() const;
    void PreUpdateComponents() const;
    void OnUpdateComponents() const;
    void PostUpdateComponents() const;
    void onUpdateSDLPollEventComponents(SDL_Event *event) const;
    void onEndComponents() const;
    void AutoLoadProjectOrContinue() const;
    void ControlFrameRate() const;
    void CaptureInputEvents(SDL_Event &e) const;
    void RegisterComponents() const;

    void PreMainLoop();

    Timer *getTimer() { return &this->timer; }
    Object3D *getObjectById(unsigned int id) const;
    Object3D *getObjectByName(const std::string &label) const;
    void removeObjectFromIndex(Object3D *obj);
    Object3D *getObjectAtScreen(int rawX, int rawY) const;

    void registerLiveObject(Object3D *obj) {
        std::lock_guard lock(livePointersMutex);
        livePointers.insert(obj);
    }
    void unregisterLiveObject(Object3D *obj) {
        std::lock_guard lock(livePointersMutex);
        livePointers.erase(obj);
    }
    // Seguro de llamar con CUALQUIER puntero, incluido uno colgante -- solo compara el valor,
    // nunca lo desreferencia. Ver comentario junto a livePointers.
    bool isValidObjectPointer(Object3D *obj) const {
        if (obj == nullptr) return false;
        std::lock_guard lock(livePointersMutex);
        return livePointers.find(obj) != livePointers.end();
    }

    float getExecutionTime() const                              { return executionTime; }
    std::vector<Object3D *> &getSceneObjects()                  { return objects; }
    std::vector<Object3D *> copySceneObjects() const {
        std::shared_lock lock(objectsMutex);
        return objects;
    }
    void lockObjects() const    { objectsMutex.lock(); }
    void unlockObjects() const  { objectsMutex.unlock(); }
    std::unique_lock<std::shared_mutex> uniqueLockObjects() { return std::unique_lock(objectsMutex); }
    std::shared_lock<std::shared_mutex> sharedLockObjects() const { return std::shared_lock(objectsMutex); }
    float getEngineTotalTime() const                            { return last_ticks / 1000.f; }
    float getDeltaTime() const                                  { return deltaTime / 1000; }
    // `deltaTime` (miembro) está en MILISEGUNDOS. getDeltaTimeMicro() devolvía deltaTime tal
    // cual (es decir, milisegundos con nombre de microsegundos) -- el binding Lua y la
    // documentación pública siempre prometieron microsegundos. Fix: multiplicar por 1000 para
    // que el contrato sea real. getDeltaTimeMS() es la accesora correcta para el uso interno en
    // milisegundos que antes usaba (por casualidad) el valor sin corregir de getDeltaTimeMicro().
    float getDeltaTimeMicro() const                             { return deltaTime * 1000.f; }
    float getDeltaTimeMS() const                                { return deltaTime; }
    Components *getComponentsManager() const                    { return componentsManager; }
    GUIManager *GUI()                                           { return &managerGUI; }
    Object3D *getObjectByIndex(int index) const;
    ThreadPool & PoolCompute()                                  { return pool; }
    ThreadPool & PoolImages()                                   { return poolImages; }
    int getPendingJobsCount() const {
        return (int)(pool.getPendingTasks() + pool.getActiveTasks() + pool.getPendingCallbacks() +
                     poolImages.getPendingTasks() + poolImages.getActiveTasks() + poolImages.getPendingCallbacks());
    }
    // Descarta trabajo encolado/callbacks pendientes de AMBOS pools -- llamar antes de recargar
    // escena (restart) para no arrastrar jobs de la partida anterior (ver doRestart() en
    // MenuManager.lua). No aborta jobs ya en ejecucion en este instante.
    void cancelPendingJobs() {
        pool.cancelPending();
        poolImages.cancelPending();
    }

    static void Shutdown()                                      { Config::get()->EXIT = true; };
    void requestExit(int code = 0)                              { Config::get()->EXIT = true; exitCode = code; }
    int getExitCode() const                                     { return exitCode; }
    const Config::LineCommandOptions &getCliOptions() const     { return cliOptions; }
    void removeAllObjects() {
        auto lock = uniqueLockObjects();
        for (auto &o : objects) o->setRemoved(true);
    }
    static unsigned int getNextUniqueObjectId();
    static std::string UniqueObjectLabel(const char *prefix);

    static Brakeza *get();
    static Brakeza *instance;
};


#endif //BRAKEDA3D_BRAKEZA3D_H
