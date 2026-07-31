#ifndef BRAKEDA3D_BRAKEZA3D_H
#define BRAKEDA3D_BRAKEZA3D_H

#include <vector>
#include <unordered_map>
#include <shared_mutex>
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
    Timer timer;

    std::vector<Object3D *> objects;
    mutable std::shared_mutex objectsMutex;
    std::unordered_map<std::string, Object3D *> objectsByName;
    std::unordered_map<unsigned int, Object3D *> objectsById;

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
    float getDeltaTimeMicro() const                             { return deltaTime; }
    Components *getComponentsManager() const                    { return componentsManager; }
    GUIManager *GUI()                                           { return &managerGUI; }
    Object3D *getObjectByIndex(int index) const;
    ThreadPool & PoolCompute()                                  { return pool; }
    ThreadPool & PoolImages()                                   { return poolImages; }
    int getPendingJobsCount() const {
        return (int)(pool.getPendingTasks() + pool.getActiveTasks() + pool.getPendingCallbacks() +
                     poolImages.getPendingTasks() + poolImages.getActiveTasks() + poolImages.getPendingCallbacks());
    }

    static void Shutdown()                                      { Config::get()->EXIT = true; };
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
