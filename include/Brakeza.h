#ifndef BRAKEDA3D_BRAKEZA3D_H
#define BRAKEDA3D_BRAKEZA3D_H

#include <vector>
#include "Components/Component.h"
#include "Components/ComponentSound.h"
#include "GUI/GUIManager.h"
#include <thread>

class Brakeza
{
    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;

    ComponentsManager *componentsManager;

    Timer engineTimer;

    std::vector<Object3D *> sceneObjects;

    GUIManager *managerGUI;
public:
    Brakeza();
    virtual ~Brakeza();

    int getNextObjectID() const;
    float &getExecutionTime();
    float getEngineTotalTime() const;
    void start(int argc, char *argv[]);
    void addObject3D(Object3D *obj, const std::string &label);
    void UpdateTimer();
    void OnStartComponents() const;
    void PreUpdateComponents() const;
    void OnUpdateComponents() const;
    void PostUpdateComponents() const;
    void onUpdateSDLPollEventComponents(SDL_Event *event) const;
    void onEndComponents() const;
    void HandleAutoStartProject(bool autostart, const std::string &project) const;
    void mainLoop(bool autostart, const std::string& project);
    void ControlFrameRate() const;
    void CaptureInputEvents(SDL_Event &e) const;

    static void shutdown();
    static void WelcomeMessage();
    static void checkForResizeOpenGLWindow(const SDL_Event &e);
    static std::string uniqueObjectLabel(const char *prefix);
    static Brakeza *get();
    static Brakeza *instance;
    Object3D *getSceneObjectById(int id) const;
    Timer *getTimer();
    [[nodiscard]] std::vector<Object3D *> &getSceneObjects();
    [[nodiscard]] float getDeltaTime() const;
    [[nodiscard]] float getDeltaTimeMicro() const;
    [[nodiscard]] ComponentsManager *getComponentsManager() const;
    [[nodiscard]] GUIManager *GUI() const;
    [[nodiscard]] Object3D *getSceneObjectByLabel(const std::string &label) const;
};


#endif //BRAKEDA3D_BRAKEZA3D_H
