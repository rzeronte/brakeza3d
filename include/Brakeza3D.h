#ifndef BRAKEDA3D_BRAKEZA3D_H
#define BRAKEDA3D_BRAKEZA3D_H

#include <vector>
#include "Objects/Object3D.h"
#include "Objects/LightPoint3D.h"
#include "Components/Component.h"
#include "ComponentsManager.h"
#include "Components/ComponentCollisions.h"
#include "LUAManager.h"
#include "Components/ComponentInput.h"
#include "Components/ComponentWindow.h"
#include "Components/ComponentSound.h"
#include "Components/ComponentRender.h"
#include "ComponentsManager.h"
#include "GUI/GUIManager.h"
#include "../sol/sol.hpp"
#include <thread>

class Brakeza3D {
private:

    ComponentsManager *componentsManager;

    Timer engineTimer;

    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;

    std::vector<Object3D *> sceneObjects;

public:
    Brakeza3D();

    virtual ~Brakeza3D();

    static Brakeza3D *get();

    static Brakeza3D *instance;

    void start();

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

    void mainLoop();

    [[nodiscard]] ComponentsManager *getComponentsManager() const;

    void controlFrameRate() const;

    float &getExecutionTime();

    static std::string uniqueObjectLabel(const char *prefix);

    static void LoadDemo();

    GUIManager *getManagerGui() const;

    static void welcomeMessage() ;

    Object3D *getSceneObjectByLabel(const std::string &label) const;

    Object3D *getSceneObjectById(int i) const;

    static void checkForResizeOpenGLWindow(SDL_Event &e);

    GUIManager *managerGUI;
    bool finish = false;
};


#endif //BRAKEDA3D_BRAKEZA3D_H
