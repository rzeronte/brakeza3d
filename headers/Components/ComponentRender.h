//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTRENDER_H
#define BRAKEDA3D_COMPONENTRENDER_H


#include <vector>
#include "Component.h"
#include "../Objects/Triangle3D.h"
#include "../Render/Tile.h"
#include "../EngineBuffers.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Physics/BillboardBody.h"
#include "../../headers/Render/Maths.h"
#include "ComponentBSP.h"
#include "ComponentWindow.h"
#include "ComponentCollisions.h"
#include "ComponentCamera.h"

struct Fragment {
    float alpha, theta, gamma, depth, affineUV, texU, texV, lightU, lightV;
};

class ComponentRender : public Component {
public:

    ComponentRender();

    void onStart();
    void preUpdate();
    void onUpdate();
    void postUpdate();
    void onEnd();
    void onSDLPollEvent(SDL_Event *event, bool &finish);

    void getBSPTriangles();
    void getObjectsTriangles();
    void hiddenSurfaceRemoval();
    void handleTrianglesToTiles(std::vector<Triangle*> &visibleTriangles);
    void handleOpenCLTriangles();
    void drawTilesGrid();
    void drawFrameTriangles(std::vector<Triangle*> &visibleTriangles);
    void processTriangle(Triangle *t);
    void triangleRasterizer(Triangle *t);
    void processPixel(Triangle *t, int bufferIndex, const int x, const int y, Fragment *);
    void drawTilesTriangles(std::vector<Triangle*> *visibleTriangles);
    void drawSceneObjectsAxis();
    void initOpenCL();
    void initTiles();
    void drawTileTriangles(int i, std::vector<Triangle*> &visibleTriangles);
    void softwareRasterizerForTile(Triangle *t, int minTileX, int minTileY, int maxTileX, int maxTileY);
    void drawWireframe(Triangle *t);

    std::vector<Triangle *> &getFrameTriangles();
    std::vector<Triangle *> &getVisibleTriangles();

    std::vector<Triangle*> frameTriangles;
    std::vector<Triangle*> clippedTriangles;
    std::vector<Triangle*> visibleTriangles;

    std::vector<Tile> tiles;
    int sizeTileWidth  = EngineSetup::getInstance()->screenWidth/2;
    int sizeTileHeight = EngineSetup::getInstance()->screenHeight/2;
    int tilesWidth;
    int tilesHeight;
    int numTiles;
    int tilePixelsBufferSize;

    // OpenCL Rasterizer
    cl_platform_id platform_id;
    cl_device_id device_cpu_id;
    cl_device_id device_gpu_id;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;
    cl_context contextCPU;
    cl_context contextGPU;
    cl_program programCPU;
    cl_program programGPU;
    cl_kernel processAllTriangles;

    cl_mem opencl_buffer_triangles;
    cl_mem opencl_buffer_video;
    cl_mem opencl_buffer_depth;

    cl_command_queue command_queue_rasterizer;
};


#endif //BRAKEDA3D_COMPONENTRENDER_H
