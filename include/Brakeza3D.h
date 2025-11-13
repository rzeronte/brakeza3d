#ifndef BRAKEDA3D_BRAKEZA3D_H
#define BRAKEDA3D_BRAKEZA3D_H

#include <vector>
#include "Objects/LightPoint3D.h"
#include "Components/Component.h"
#include "Components/ComponentSound.h"
#include "GUI/GUIManager.h"
#include <thread>

class Brakeza3D {

    ComponentsManager *componentsManager;

    Timer engineTimer;

    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;

    std::vector<Object3D *> sceneObjects;

    GUIManager *managerGUI;
public:
    Brakeza3D();

    virtual ~Brakeza3D();

    static Brakeza3D *get();

    static Brakeza3D *instance;

    void start(int argc, char *argv[]);

    void addObject3D(Object3D *obj, const std::string &label);

    [[nodiscard]] std::vector<Object3D *> &getSceneObjects();

    Timer *getTimer();

    void updateTimer();

    [[nodiscard]] float getDeltaTime() const;

    [[nodiscard]] float getDeltaTimeMicro() const;

    void onStartComponents();

    void preUpdateComponents();

    void onUpdateComponents();

    void postUpdateComponents();

    void onUpdateSDLPollEventComponents(SDL_Event *event, bool &finish);

    void onEndComponents();

    void mainLoop(bool autostart, const std::string& project);

    [[nodiscard]] ComponentsManager *getComponentsManager() const;

    void controlFrameRate() const;

    float &getExecutionTime();

    static std::string uniqueObjectLabel(const char *prefix);

    [[nodiscard]] GUIManager *getManagerGui() const;

    static void welcomeMessage();

    [[nodiscard]] Object3D *getSceneObjectByLabel(const std::string &label) const;

    static void checkForResizeOpenGLWindow(SDL_Event &e);

    bool finish = false;

    void shutdown();
};


#endif //BRAKEDA3D_BRAKEZA3D_H
