//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTRENDER_H
#define BRAKEDA3D_COMPONENTRENDER_H

#define MAX_OPENCL_LIGHTS 16

#include <vector>
#include <mutex>
#include "Component.h"
#include "../Objects/Triangle3D.h"
#include "../Render/Tile.h"
#include "../EngineBuffers.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Physics/BillboardBody.h"
#include "../../include/Render/Maths.h"
#include "../Shaders/ShaderBilinear.h"
#include "../Shaders/ShaderDepthOfField.h"
#include "../../src/Shaders/ShaderParticlesBlurBuffer.h"
#include "../2D/TextWriter.h"
#include "../Misc/SharedLUAContext.h"
#include "../Misc/SceneLoader.h"
#include "../Shaders/ShaderImage.h"
#include <CL/cl.h>

class ComponentRender : public Component {
private:

    int fps;
    int fpsFrameCounter;
    float frameTime;

    std::string defaultGPU;

    std::vector<Triangle *> frameTriangles;
    std::vector<Triangle *> clippedTriangles;
    std::vector<Triangle *> visibleTriangles;
    std::vector<Triangle *> spritesTriangles;
    std::vector<LightPoint3D *> lightPoints;

    std::vector<Tile> tiles;
    int sizeTileWidth;
    int sizeTileHeight;
    int tilesWidth;
    int tilesHeight;
    int numTiles;
    int tilePixelsBufferSize;

    Object3D *selectedObject;

    cl_platform_id clPlatformId;
    cl_device_id clDeviceId;

    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;
    cl_context clContext;

    cl_command_queue clCommandQueue;

    cl_program rendererProgram;
    cl_kernel rendererKernel;

    cl_program particlesProgram;
    cl_kernel particlesKernel;
    cl_mem clBufferVideoParticles;

    cl_program explosionProgram;
    cl_kernel explosionKernel;

    cl_program blinkProgram;
    cl_kernel blinkKernel;

    cl_program edgeProgram;
    cl_kernel edgeKernel;

    cl_mem clBufferLights;
    std::vector<OCLight> oclLights;


    EngineSetup::LuaStateScripts stateScripts;
    std::vector<ScriptLUA*> scripts;
    TextWriter *textWriter;
    SceneLoader sceneLoader;

    bool sceneShadersEnabled;
    std::vector<ShaderOpenCL*> sceneShaders;

public:
    ComponentRender();

    ~ComponentRender() override;

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    void onUpdateSceneObjects();

    void hiddenSurfaceRemoval();

    void hiddenSurfaceRemovalTriangle(Triangle *t);

    void hiddenSurfaceRemovalTriangleForLight(Triangle *t, LightPoint3D *l, std::vector<Triangle *> &visibleTrianglesForLight, std::vector<Triangle *> &clippedTriangles);

    void hiddenOctreeRemoval();

    void hiddenOctreeRemovalNode(OctreeNode *node, std::vector<Triangle *> &triangles);

    void drawVisibleTriangles();

    void handleTrianglesToTiles(std::vector<Triangle *> &triangles);

    void drawTilesGrid();

    void drawTriangles(std::vector<Triangle *> &visibleTriangles);

    void render(Triangle *t);

    void triangleRasterizer(Triangle *t);

    void triangleRasterizerForDepthMapping(Triangle *t, LightPoint3D *ligthpoint);

    std::vector<LightPoint3D *> &getLightPoints();

    void processPixel(Triangle *t, int bufferIndex, const int x, const int y, Fragment *, bool bilinear);

    void drawTilesTriangles(std::vector<Triangle *> *visibleTriangles);

    void initTiles();

    void drawTileTriangles(int i, std::vector<Triangle *> &trianglesToDraw);

    void softwareRasterizerForTile(Triangle *t, int minTileX, int minTileY, int maxTileX, int maxTileY);

    static void drawWireframe(Triangle *t);

    Color processPixelFog(Fragment *fragment, Color pixelColor);

    Color processPixelLights(Triangle *t, Fragment *f, Color c);

    void updateLights();

    void updateFPS();

    std::vector<Triangle *> &getFrameTriangles();

    std::vector<Triangle *> &getVisibleTriangles();

    std::vector<Triangle *> &getSpritesTriangles();

    void extractLightPointsFromObjects3D();

    void createLightPointsDepthMappings();

    [[nodiscard]] Object3D* getSelectedObject() const;

    void setSelectedObject(Object3D *o);

    void initOpenCL();

    Object3D *getObject3DFromClickPoint(int xClick, int yClick);

    void updateSelectedObject3D();

    void onPostUpdateSceneObjects();

    void OpenCLInfo();

    std::vector<Tile> &getTiles();

    [[nodiscard]] int getTilesWidth() const;

    [[nodiscard]] int getTilesHeight() const;

    [[nodiscard]] int getNumTiles() const;

    [[nodiscard]] int getFps();

    _cl_platform_id *getClPlatformId();

    _cl_device_id *getClDeviceId();

    _cl_context *getClContext();

    _cl_command_queue *getClCommandQueue();

    cl_device_id selectDefaultGPUDevice();

    void writeOCLBuffersFromHost() const;

    void writeOCLBufferIntoHost() const;

    _cl_program *getRendererProgram();

    _cl_kernel *getRendererKernel();

    void drawObjetsInHostBuffer();

    void deleteRemovedObjects();

    _cl_kernel *getParticlesKernel();

    void loadKernel(cl_program &program, cl_kernel &kernel, const std::string& source);

    _cl_kernel *getExplosionKernel();

    _cl_kernel *getBlinkKernel();

    void onUpdateSceneObjectsSecondPass() const;

    cl_mem *getClBufferLights();

    void updateLightsOCL();

    cl_mem *getClBufferVideoParticles();

    void loadConfig();

    void loadCommonKernels();

    EngineSetup::LuaStateScripts getStateLUAScripts();

    void playLUAScripts();

    void stopLUAScripts();

    void reloadLUAScripts();

    std::vector<ScriptLUA*> &getLUAScripts();

    void addLUAScript(ScriptLUA *script);

    void reloadScriptGlobals();

    void removeScript(ScriptLUA *script);

    void onStartScripts();

    void runScripts();

    SceneLoader &getSceneLoader();

    std::vector<ShaderOpenCL *> &getSceneShaders();

    void addShaderToScene(ShaderOpenCL *shader);

    [[nodiscard]] bool isSceneShadersEnabled() const;

    void setSceneShadersEnabled(bool sceneShadersEnabled);

    void runShadersOpenCLPostUpdate();

    void removeShader(int index);

    void runShadersOpenCLPreUpdate();

    _cl_kernel *getEdgeKernel();

    ShaderOpenCL *getSceneShaderByIndex(int i);
};


#endif //BRAKEDA3D_COMPONENTRENDER_H
