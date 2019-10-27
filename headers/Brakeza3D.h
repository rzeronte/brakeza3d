#ifndef BRAKEDA3D_BRAKEZA3D_H
#define BRAKEDA3D_BRAKEZA3D_H

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include <vector>
#include "Objects/Object3D.h"
#include "Objects/LightPoint3D.h"
#include "Collisions/CollisionsManager.h"
#include "Render/InputController.h"
#include "GUI/GUIManager.h"
#include "Collisions/CollisionResolver.h"

class Brakeza3D {
private:
    // Scene 3D objects
    std::vector<Object3D*> sceneObjects;
    // Camera
    Camera3D *camera;
    // Input InputController
    InputController *controllerManager;
    // GUI
    GUIManager *guiManager;
    // BSP map
    BSPMap *bsp;
    // Lights
    std::vector<LightPoint3D *> lightPoints;
    // collision Manager
    CollisionsManager *collisionsManager;
    // weaponManager
    WeaponsManager *weaponManager;
    // menuManager
    MenuManager *menuManager;
    // Timer
    Timer engineTimer;
public:
    Brakeza3D();
    ~Brakeza3D();

    static Brakeza3D* get();
    static Brakeza3D* instance;

    // SDL GL_Context for GUI
    SDL_GLContext gl_context;

    // Window and Renderer
    SDL_Window *window = NULL;
    SDL_Renderer *renderer;

    SDL_Surface *screenSurface;
    SDL_Texture *screenTexture;

    TTF_Font *font = NULL;

    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;
    float frameTime = 0;

    // Fps counter
    int fps = 0;
    int fpsFrameCounter = 0;

    // start Brakeza3D
    void start();

    bool initSound();
    bool initWindow();
    void initFontsTTF();

    // loader for BSP maps
    void initBSP(const char *bspFilename, std::vector<Triangle*> *frameTriangles);

    // Objects3D Managing
    void      addObject3D(Object3D *obj, std::string label);
    Object3D* getObjectByLabel(std::string label);
    void      addLightPoint(LightPoint3D *lightPoint, std::string label);

    Camera3D *getCamera();

    std::vector<Object3D*>     &getSceneObjects();
    std::vector<LightPoint3D*> &getLightPoints();

    InputController   *getController();
    CollisionsManager *getCollisionManager();
    GUIManager        *getGUIManager();
    MenuManager       *getMenuManager();
    WeaponsManager    *getWeaponsManager();

    BSPMap *getBSP();

    // Global timer
    Timer* getTimer();
    void   updateTimer();
    float  getDeltaTime();

    void updateFPS();

    void drawMenuScreen();

    void waterShader();

    void triangleRasterizer(Triangle *t);
    void processPixel(Triangle *t, int bufferIndex, int x, int y, float w0, float w1, float w2, float z, float texu, float texv, float lightu, float lightv);
    void softwareRasterizerForTile(Triangle *t, int minTileX, int minTileY, int maxTileX, int maxTileY);
};


#endif //BRAKEDA3D_BRAKEZA3D_H
