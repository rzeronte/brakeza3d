#ifndef BRAKEDA3D_BRAKEZA3D_H
#define BRAKEDA3D_BRAKEZA3D_H

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include <vector>
#include "Objects/Object3D.h"
#include "Objects/LightPoint3D.h"
#include "Components/ComponentCollisions.h"
#include "Input/InputController.h"
#include "GUI/GUIManager.h"
#include "Collisions/CollisionResolver.h"
#include "EngineBuffers.h"
#include "Components/Component.h"
#include "Components/ComponentInput.h"
#include "Components/ComponentWindow.h"
#include "Components/ComponentBSP.h"
#include "Components/ComponentSound.h"
#include "Components/ComponentRender.h"
#include "Components/ComponentHUD.h"
#include "Components/ComponentsManager.h"
#include "Components/ComponentGUI.h"
#include <thread>

class Brakeza3D {
private:
    std::vector<Object3D*>   sceneObjects;

    ComponentsManager *componentsManager;

    ComponentCamera     *componentCamera;
    ComponentCollisions *componentCollisions;
    ComponentWeapons    *componentWeapons;
    ComponentMenu       *componentMenu;
    ComponentInput      *componentInput;
    ComponentWindow     *componentWindow;
    ComponentBSP        *componentBSP;
    ComponentSound      *componentSound;
    ComponentRender     *componentRender;
    ComponentHUD        *componentHUD;
    ComponentGUI        *componentGUI;

    BSPMap      *bsp;
    EngineSetup *setup;

    GUIManager *guiManager;
    InputController *controllerManager;

    // Timer
    Timer engineTimer;

    std::thread *loadingBSP;

public:
    Brakeza3D();
    ~Brakeza3D();

    static Brakeza3D* get();
    static Brakeza3D* instance;

    bool finish = false;

    SDL_Window   *window;
    SDL_Renderer *renderer;

    SDL_Surface *screenSurface;
    SDL_Texture *screenTexture;

    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;
    float frameTime = 0;

    // Fps counter
    int fps = 0;
    int fpsFrameCounter = 0;

    float currentFadePercent = 1;

    // start Brakeza3D
    void start();

    // Objects3D Managing
    void      addObject3D(Object3D *obj, std::string label);
    Object3D* getObjectByLabel(std::string label);

    std::vector<Object3D*> &getSceneObjects();

    Timer* getTimer();
    void   updateTimer();
    float  getDeltaTime();

    void updateFPS();

    void onStartComponents();
    void preUpdateComponents();
    void onUpdateComponents();
    void postUpdateComponents();
    void onUpdateSDLPollEventComponents(SDL_Event *event, bool &finish);
    void onEndComponents();
    void mainLoop();

    GUIManager* getGUIManager();

    ComponentsManager *getComponentsManager() const;
};



#endif //BRAKEDA3D_BRAKEZA3D_H
