#ifndef BRAKEDA3D_BRAKEZA3D_H
#define BRAKEDA3D_BRAKEZA3D_H

#include <vector>
#include "Objects/Object3D.h"
#include "Objects/LightPoint3D.h"
#include "Components/Component.h"
#include "ComponentsManager.h"
#include "Components/ComponentCollisions.h"
#include "EngineBuffers.h"
#include "Components/ComponentInput.h"
#include "Components/ComponentWindow.h"
#include "Components/ComponentSound.h"
#include "Components/ComponentRender.h"
#include "Components/ComponentHUD.h"
#include "ComponentsManager.h"
#include "Components/ComponentGame.h"
#include "Components/ComponentGameInput.h"
#include "GUI/GUIManager.h"
#include <thread>

class Brakeza3D {
private:
    GUIManager managerGUI;

    ComponentsManager *componentsManager;

    ComponentCamera *componentCamera = nullptr;
    ComponentCollisions *componentCollisions = nullptr;
    ComponentMenu *componentMenu = nullptr;
    ComponentInput *componentInput = nullptr;
    ComponentWindow *componentWindow = nullptr;
    ComponentSound *componentSound = nullptr;
    ComponentRender *componentRender = nullptr;
    ComponentHUD *componentHUD = nullptr;
    ComponentGame *componentGame = nullptr;
    ComponentGameInput *componentGameInput = nullptr;

    Timer engineTimer;

    bool finish = false;

    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;

    float currentFadePercent = 1;

    std::vector<Object3D *> sceneObjects;
public:
    Brakeza3D();

    virtual ~Brakeza3D();

    static Brakeza3D *get();

    static Brakeza3D *instance;

    void start();

    void addObject3D(Object3D *obj, const std::string &label);

    [[nodiscard]] const std::vector<Object3D *> &getSceneObjects() const;

    Object3D *getObjectByLabel(const std::string &label);

    Timer *getTimer();

    void updateTimer();

    [[nodiscard]] float getDeltaTime() const;

    void onStartComponents();

    void preUpdateComponents();

    void onUpdateComponents();

    void postUpdateComponents();

    void onUpdateSDLPollEventComponents(SDL_Event *event, bool &finish);

    void onEndComponents();

    void mainLoop();

    [[nodiscard]] ComponentsManager *getComponentsManager() const;

    void AxisPlaneInitialize();

    void ImGuiInitialize() const;

    void ImGuiOnUpdate();

    void controlFrameRate() const;

    float &getExecutionTime();
};


#endif //BRAKEDA3D_BRAKEZA3D_H
