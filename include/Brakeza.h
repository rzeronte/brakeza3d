#ifndef BRAKEDA3D_BRAKEZA3D_H
#define BRAKEDA3D_BRAKEZA3D_H

#include <vector>
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

    GUIManager managerGUI;
    ThreadPool pool;
    ThreadPool poolImages;

public:
    Brakeza();
    virtual ~Brakeza();

    bool ReadArgs(int argc, char **argv);
    void Start(int argc, char *argv[]);
    void MainLoop();
    void addObject3D(Object3D *obj, const std::string &label);
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
    Object3D *getObjectById(int id) const;
    Object3D *getObjectByName(const std::string &label) const;
    int getNextUniqueObjectId() const;

    float getExecutionTime() const                              { return executionTime; }
    std::vector<Object3D *> &getSceneObjects()                  { return objects; }
    float getEngineTotalTime() const                            { return last_ticks / 1000.f; }
    float getDeltaTime() const                                  { return deltaTime / 1000; }
    float getDeltaTimeMicro() const                             { return deltaTime; }
    Components *getComponentsManager() const                    { return componentsManager; }
    GUIManager *GUI()                                           { return &managerGUI; }
    Object3D *getObjectByIndex(int index) const                 { return objects[index]; }
    ThreadPool & Pool()                                         { return pool; }
    ThreadPool & PoolImages()                                   { return pool; }

    static void Shutdown()                                      { Config::get()->EXIT = true; };
    static std::string UniqueObjectLabel(const char *prefix);

    static Brakeza *get();
    static Brakeza *instance;
};


#endif //BRAKEDA3D_BRAKEZA3D_H
