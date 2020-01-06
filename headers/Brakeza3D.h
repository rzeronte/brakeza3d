#ifndef BRAKEDA3D_BRAKEZA3D_H
#define BRAKEDA3D_BRAKEZA3D_H

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include <vector>
#include "Objects/Object3D.h"
#include "Objects/LightPoint3D.h"
#include "Collisions/CollisionsManager.h"
#include "Input/InputController.h"
#include "GUI/GUIManager.h"
#include "Collisions/CollisionResolver.h"
#include "EngineBuffers.h"
#include <thread>

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

    EngineSetup   *setup;
    EngineBuffers *buffer;

    // Timer
    Timer engineTimer;

    std::thread *loadingBSP;

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

    TTF_Font *fontSmall   = NULL;
    TTF_Font *fontDefault = NULL;
    TTF_Font *fontMedium  = NULL;
    TTF_Font *fontBig     = NULL;

    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;
    float frameTime = 0;

    // Fps counter
    int fps = 0;
    int fpsFrameCounter = 0;

    std::vector<Tile> tiles;
    int sizeTileWidth  = EngineSetup::getInstance()->screenWidth/2;
    int sizeTileHeight = EngineSetup::getInstance()->screenHeight/2;
    int tilesWidth;
    int tilesHeight;
    int numTiles;
    int tilePixelsBufferSize;

    float currentFadePercent = 1;

    // start Brakeza3D
    void start();

    bool initSound();
    bool initWindow();
    void initFontsTTF();
    void initTiles();

    // loader for BSP maps
    void initBSP(const char *bspFilename, std::vector<Triangle*> *frameTriangles);
    void setCameraInBSPStartPosition();

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

    void setController(InputController *);

    BSPMap *getBSP();

    // Global timer
    Timer* getTimer();
    void  updateTimer();
    float getDeltaTime();

    void updateFPS();

    // Draw triangles
    void drawTilesTriangles(std::vector<Triangle*> *visibleTriangles);
    void drawFrameTriangles(std::vector<Triangle*> &visibleTriangles);

    void processTriangle(Triangle *t);
    void triangleRasterizer(Triangle *t);
    void processPixel(Triangle *t, int bufferIndex, int x, int y, float w0, float w1, float w2, float z, float texu, float texv, float lightu, float lightv);
    void softwareRasterizerForTile(Triangle *t, int minTileX, int minTileY, int maxTileX, int maxTileY);
    void drawWireframe(Triangle *t);
    void drawWireframeColor(Triangle *t, Uint32 c);

    void drawTileTriangles(int i, std::vector<Triangle*> &);
    void handleTrianglesToTiles(std::vector<Triangle*> &visibleTriangles);
    void drawTilesGrid();

    void drawSplash(SDL_Surface *surface);

    Counter *splashCounter;
};



#endif //BRAKEDA3D_BRAKEZA3D_H
