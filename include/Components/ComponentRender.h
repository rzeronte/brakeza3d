//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTRENDER_H
#define BRAKEDA3D_COMPONENTRENDER_H

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <vector>
#include <mutex>
#include "Component.h"
#include "../Objects/Triangle3D.h"
#include "../Render/Tile.h"
#include "../EngineBuffers.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Physics/BillboardBody.h"
#include "../../include/Render/Maths.h"
#include "ComponentWindow.h"
#include "ComponentCollisions.h"
#include "ComponentCamera.h"
#include "../../darkheaz/src/shaders/ShaderBilinear.h"
#include <CL/cl.h>

class ComponentRender : public Component {
private:

    int fps;
    int fpsFrameCounter;
    float frameTime;

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

    ShaderBilinear *shaderBilinear;
public:
    ComponentRender();

    virtual ~ComponentRender();

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

    void drawSceneOverlappingItems();

    void initTiles();

    void drawTileTriangles(int i, std::vector<Triangle *> &trianglesToDraw);

    void softwareRasterizerForTile(Triangle *t, int minTileX, int minTileY, int maxTileX, int maxTileY);

    static void drawWireframe(Triangle *t);

    Color processPixelFog(Fragment *fragment, Color pixelColor);

    Color processPixelLights(Triangle *t, Fragment *f, Color c);

    void updateLights();

    void updateFPS(float deltaTime);

    std::vector<Triangle *> &getFrameTriangles();

    std::vector<Triangle *> &getVisibleTriangles();

    std::vector<Triangle *> &getSpritesTriangles();

    void extractLightPointsFromObjects3D();

    void createLightPointsDepthMappings();

    std::string getUniqueGameObjectLabel();

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

    cl_device_id selectNvidiaDevice();

    void writeOCLBuffersFromHost() const;

    void writeOCLBufferIntoHost() const;

    void loadRenderKernel();

    _cl_program *getRendererProgram();

    _cl_kernel *getRendererKernel();

    void drawObjetsInHostBuffer();

    void deleteRemovedObjects();

    void loadParticlesKernel();

    _cl_program *getParticlesProgram();

    _cl_kernel *getParticlesKernel();
};


#endif //BRAKEDA3D_COMPONENTRENDER_H
